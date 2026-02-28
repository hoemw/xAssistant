#include "Tools/ActorTools.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "JsonObjectConverter.h"

// Editor-only includes
#include "Editor.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"
#include "ScopedTransaction.h"
#include "Engine/Selection.h"
#include "Engine/PointLight.h"
#include "Engine/DirectionalLight.h"
#include "Engine/StaticMeshActor.h"
#include "Tools/SUEAEditorHelpers.h"


static AActor* FindActorByNameFuzzy(UWorld* World, const FString& Query, bool bExact)
{
    if (!World) return nullptr;
    for (TActorIterator<AActor> It(World); It; ++It)
    {
        AActor* Actor = *It;
        const FString Name = Actor->GetName();
        if (bExact)
        {
            if (Name.Equals(Query, ESearchCase::IgnoreCase))
            {
                return Actor;
            }
        }
        else
        {
            if (Name.Contains(Query, ESearchCase::IgnoreCase))
            {
                return Actor;
            }
        }
    }
    return nullptr;
}

// ---------------- FSelectAndFocusActorTool ----------------
FSelectAndFocusActorTool::FSelectAndFocusActorTool()
{
    Spec.Name = TEXT("select_focus_actor");
    Spec.Description = TEXT("选择并聚焦指定Actor（按名称模糊匹配）");
    Spec.Params = {
        {TEXT("query"), TEXT("string"), false, TEXT("待查找的Actor名称关键词或完整名")},
        {TEXT("exact"), TEXT("boolean"), true, TEXT("是否精确匹配，默认false")}
    };
    Spec.Permission = EToolPermission::Safe;
}

FAIToolResult FSelectAndFocusActorTool::Execute(const TSharedPtr<FJsonObject>& Args)
{
    if (!Args.IsValid() || !Args->HasTypedField<EJson::String>(TEXT("query")))
    {
        return {false, TEXT("缺少参数：query"), nullptr};
    }
    const FString Query = Args->GetStringField(TEXT("query"));
    const bool bExact = Args->HasField(TEXT("exact")) ? Args->GetBoolField(TEXT("exact")) : false;

    UWorld* World = SUEA::GetEditorWorld();
    if (!World) return {false, TEXT("未获取到编辑器世界"), nullptr};

    AActor* Target = FindActorByNameFuzzy(World, Query, bExact);
    if (!Target)
    {
        return {false, FString::Printf(TEXT("未找到匹配的Actor：%s"), *Query), nullptr};
    }

    if (GEditor)
    {
        GEditor->SelectNone(true, true, false);
        GEditor->SelectActor(Target, true, true, true);

        TArray<AActor*> Actors; Actors.Add(Target);
        GEditor->MoveViewportCamerasToActor(Actors, false);
        GEditor->NoteSelectionChange();
    }

    TSharedPtr<FJsonObject> Data = MakeShareable(new FJsonObject);
    Data->SetStringField(TEXT("actor"), Target->GetName());
    Data->SetStringField(TEXT("class"), Target->GetClass()->GetName());
    Data->SetStringField(TEXT("location"), Target->GetActorLocation().ToString());

    return {true, TEXT("已选择并聚焦目标Actor"), Data};
}

// ---------------- FSetActorTransformTool ----------------
FSetActorTransformTool::FSetActorTransformTool()
{
    Spec.Name = TEXT("set_actor_transform");
    Spec.Description = TEXT("设置Actor的位置/旋转/缩放（支持部分字段）");
    Spec.Params = {
        {TEXT("name"), TEXT("string"), false, TEXT("Actor名称（默认精确匹配）")},
        {TEXT("location"), TEXT("object"), true, TEXT("{x,y,z}，单位cm")},
        {TEXT("rotation"), TEXT("object"), true, TEXT("{pitch,yaw,roll}，单位度")},
        {TEXT("scale"), TEXT("object"), true, TEXT("{x,y,z}")}
    };
    Spec.Permission = EToolPermission::Modify;
}

FAIToolResult FSetActorTransformTool::Execute(const TSharedPtr<FJsonObject>& Args)
{
    if (!Args.IsValid() || !Args->HasTypedField<EJson::String>(TEXT("name")))
    {
        return {false, TEXT("缺少参数：name"), nullptr};
    }
    UWorld* World = SUEA::GetEditorWorld();
    if (!World) return {false, TEXT("未获取到编辑器世界"), nullptr};

    const FString Name = Args->GetStringField(TEXT("name"));
    AActor* Target = FindActorByNameFuzzy(World, Name, /*bExact*/true);
    if (!Target) return {false, FString::Printf(TEXT("未找到Actor：%s"), *Name), nullptr};

    const FScopedTransaction Tx(NSLOCTEXT("SmartUE", "SetActorTransformTx", "AI: Set Actor Transform"));
    Target->Modify();

    // location
    if (Args->HasTypedField<EJson::Object>(TEXT("location")))
    {
        const TSharedPtr<FJsonObject> L = Args->GetObjectField(TEXT("location"));
        FVector Loc = Target->GetActorLocation();
        if (L->HasTypedField<EJson::Number>(TEXT("x"))) Loc.X = L->GetNumberField(TEXT("x"));
        if (L->HasTypedField<EJson::Number>(TEXT("y"))) Loc.Y = L->GetNumberField(TEXT("y"));
        if (L->HasTypedField<EJson::Number>(TEXT("z"))) Loc.Z = L->GetNumberField(TEXT("z"));
        Target->SetActorLocation(Loc, false, nullptr, ETeleportType::None);
    }

    // rotation
    if (Args->HasTypedField<EJson::Object>(TEXT("rotation")))
    {
        const TSharedPtr<FJsonObject> R = Args->GetObjectField(TEXT("rotation"));
        FRotator Rot = Target->GetActorRotation();
        if (R->HasTypedField<EJson::Number>(TEXT("pitch"))) Rot.Pitch = R->GetNumberField(TEXT("pitch"));
        if (R->HasTypedField<EJson::Number>(TEXT("yaw"))) Rot.Yaw = R->GetNumberField(TEXT("yaw"));
        if (R->HasTypedField<EJson::Number>(TEXT("roll"))) Rot.Roll = R->GetNumberField(TEXT("roll"));
        Target->SetActorRotation(Rot, ETeleportType::None);
    }

    // scale
    if (Args->HasTypedField<EJson::Object>(TEXT("scale")))
    {
        const TSharedPtr<FJsonObject> S = Args->GetObjectField(TEXT("scale"));
        FVector Scale = Target->GetActorScale3D();
        if (S->HasTypedField<EJson::Number>(TEXT("x"))) Scale.X = S->GetNumberField(TEXT("x"));
        if (S->HasTypedField<EJson::Number>(TEXT("y"))) Scale.Y = S->GetNumberField(TEXT("y"));
        if (S->HasTypedField<EJson::Number>(TEXT("z"))) Scale.Z = S->GetNumberField(TEXT("z"));
        Target->SetActorScale3D(Scale);
    }

    Target->PostEditChange();
    Target->MarkPackageDirty();
    if (GEditor) GEditor->RedrawLevelEditingViewports(true);

    TSharedPtr<FJsonObject> Data = MakeShareable(new FJsonObject);
    Data->SetStringField(TEXT("actor"), Target->GetName());
    Data->SetStringField(TEXT("location"), Target->GetActorLocation().ToString());
    Data->SetStringField(TEXT("rotation"), Target->GetActorRotation().ToString());
    Data->SetStringField(TEXT("scale"), Target->GetActorScale3D().ToString());

    return {true, TEXT("已更新Actor变换"), Data};
}

// ---------------- FCreateActorBasicTool ----------------
FCreateActorBasicTool::FCreateActorBasicTool()
{
    Spec.Name = TEXT("create_actor_basic");
    Spec.Description = TEXT("创建基础Actor：EmptyActor、PointLight、DirectionalLight、StaticMeshActor(默认引擎立方体)等");
    Spec.Params = {
        {TEXT("type"), TEXT("string"), false, TEXT("empty | point_light | directional_light | cube")},
        {TEXT("name"), TEXT("string"), true, TEXT("可选自定义名称")},
        {TEXT("location"), TEXT("object"), true, TEXT("{x,y,z} 初始位置，可选")}
    };
    Spec.Permission = EToolPermission::Modify;
}

FAIToolResult FCreateActorBasicTool::Execute(const TSharedPtr<FJsonObject>& Args)
{
    if (!Args.IsValid() || !Args->HasTypedField<EJson::String>(TEXT("type")))
        return {false, TEXT("缺少参数：type"), nullptr};

    UWorld* World = SUEA::GetEditorWorld();
    if (!World) return {false, TEXT("未获取到编辑器世界"), nullptr};

    const FString Type = Args->GetStringField(TEXT("type"));
    const FString CustomName = Args->HasTypedField<EJson::String>(TEXT("name")) ? Args->GetStringField(TEXT("name")) : TEXT("");

    FActorSpawnParameters Params; Params.Name = CustomName.IsEmpty() ? NAME_None : FName(*CustomName);
    FVector Loc(0,0,0);
    if (Args->HasTypedField<EJson::Object>(TEXT("location")))
    {
        const TSharedPtr<FJsonObject> L = Args->GetObjectField(TEXT("location"));
        if (L->HasTypedField<EJson::Number>(TEXT("x"))) Loc.X = L->GetNumberField(TEXT("x"));
        if (L->HasTypedField<EJson::Number>(TEXT("y"))) Loc.Y = L->GetNumberField(TEXT("y"));
        if (L->HasTypedField<EJson::Number>(TEXT("z"))) Loc.Z = L->GetNumberField(TEXT("z"));
    }

    AActor* NewActor = nullptr;
    if (Type.Equals(TEXT("empty"), ESearchCase::IgnoreCase))
    {
        NewActor = World->SpawnActor<AActor>(AActor::StaticClass(), Loc, FRotator::ZeroRotator, Params);
    }
    else if (Type.Equals(TEXT("point_light"), ESearchCase::IgnoreCase))
    {
        APointLight* Light = World->SpawnActor<APointLight>(APointLight::StaticClass(), Loc, FRotator::ZeroRotator, Params);
        NewActor = Light;
    }
    else if (Type.Equals(TEXT("directional_light"), ESearchCase::IgnoreCase))
    {
        ADirectionalLight* Light = World->SpawnActor<ADirectionalLight>(ADirectionalLight::StaticClass(), Loc, FRotator::ZeroRotator, Params);
        NewActor = Light;
    }
    else if (Type.Equals(TEXT("cube"), ESearchCase::IgnoreCase))
    {
        AStaticMeshActor* MeshActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Loc, FRotator::ZeroRotator, Params);
        NewActor = MeshActor;
    }

    if (!NewActor) return {false, TEXT("不支持的type或生成失败"), nullptr};

    TSharedPtr<FJsonObject> Data = MakeShareable(new FJsonObject);
    Data->SetStringField(TEXT("name"), NewActor->GetName());
    Data->SetStringField(TEXT("class"), NewActor->GetClass()->GetName());
    Data->SetStringField(TEXT("location"), NewActor->GetActorLocation().ToString());
    return {true, TEXT("已创建Actor"), Data};
}

// ---------------- FDeleteActorTool ----------------
FDeleteActorTool::FDeleteActorTool()
{
    Spec.Name = TEXT("delete_actor");
    Spec.Description = TEXT("删除指定名称的Actor（精确匹配）");
    Spec.Params = {
        {TEXT("name"), TEXT("string"), false, TEXT("Actor名称（精确匹配）")}
    };
    Spec.Permission = EToolPermission::Modify;
    Spec.bRequireConfirm = true;
}

FAIToolResult FDeleteActorTool::Execute(const TSharedPtr<FJsonObject>& Args)
{
    if (!Args.IsValid() || !Args->HasTypedField<EJson::String>(TEXT("name")))
        return {false, TEXT("缺少参数：name"), nullptr};

    UWorld* World = SUEA::GetEditorWorld();
    if (!World) return {false, TEXT("未获取到编辑器世界"), nullptr};

    const FString Name = Args->GetStringField(TEXT("name"));
    AActor* Target = FindActorByNameFuzzy(World, Name, /*bExact*/true);
    if (!Target) return {false, FString::Printf(TEXT("未找到Actor：%s"), *Name), nullptr};

    const FScopedTransaction Tx(NSLOCTEXT("SmartUE", "DeleteActorTx", "AI: Delete Actor"));
    Target->Modify();
    bool bDestroyed = Target->Destroy();

    return {bDestroyed, bDestroyed ? TEXT("已删除Actor") : TEXT("删除失败"), nullptr};
}

static bool CollectTargetActors(UWorld* World, const TSharedPtr<FJsonObject>& Args, TArray<AActor*>& OutTargets)
{
    if (!World) return false;

    // 1) 显式 targets
    if (Args.IsValid() && Args->HasTypedField<EJson::Array>(TEXT("targets")))
    {
        const TArray<TSharedPtr<FJsonValue>>& Arr = Args->GetArrayField(TEXT("targets"));
        for (const auto& V : Arr)
        {
            FString Name;
            if (V.IsValid() && V->TryGetString(Name))
            {
                for (TActorIterator<AActor> It(World); It; ++It)
                {
                    if ((*It)->GetName().Equals(Name, ESearchCase::IgnoreCase))
                    {
                        OutTargets.Add(*It);
                        break;
                    }
                }
            }
        }
    }

    // 2) 若无 targets，使用当前选择集
    if (OutTargets.Num() == 0 && GEditor)
    {
        USelection* Sel = GEditor->GetSelectedActors();
        if (Sel)
        {
            for (FSelectionIterator It(*Sel); It; ++It)
            {
                if (AActor* A = Cast<AActor>(*It)) OutTargets.Add(A);
            }
        }
    }

    // 3) 若仍为空，按 name_contains / class 过滤所有Actor
    if (OutTargets.Num() == 0 && Args.IsValid())
    {
        const FString NameFilter = Args->HasTypedField<EJson::String>(TEXT("name_contains")) ? Args->GetStringField(TEXT("name_contains")) : TEXT("");
        const FString ClassFilter = Args->HasTypedField<EJson::String>(TEXT("class")) ? Args->GetStringField(TEXT("class")) : TEXT("");
        for (TActorIterator<AActor> It(World); It; ++It)
        {
            AActor* A = *It;
            if (!NameFilter.IsEmpty() && !A->GetName().Contains(NameFilter, ESearchCase::IgnoreCase)) continue;
            if (!ClassFilter.IsEmpty() && !A->GetClass()->GetName().Contains(ClassFilter, ESearchCase::IgnoreCase)) continue;
            OutTargets.Add(A);
        }
    }

    // 去重
    {
        TSet<AActor*> Set;
        for (AActor* A : OutTargets)
        {
            if (IsValid(A)) Set.Add(A);
        }
        OutTargets.Reset(Set.Num());
        for (AActor* A : Set)
        {
            OutTargets.Add(A);
        }
        OutTargets.Sort([](const AActor& L, const AActor& R)
        {
            return L.GetName() < R.GetName();
        });
    }
 
     return OutTargets.Num() > 0;
}

// ---------------- FTransformActorsDeltaTool ----------------
FTransformActorsDeltaTool::FTransformActorsDeltaTool()
{
    Spec.Name = TEXT("transform_actors_delta");
    Spec.Description = TEXT("对多个Actor进行相对变换：位移/旋转/缩放增量，支持world/local空间，支持作用于选择集或按名称过滤");
    Spec.Params = {
        {TEXT("targets"), TEXT("array"), true, TEXT("可选：显式Actor名称数组，若省略则使用当前选择集；也可配合 name_contains/class 过滤")},
        {TEXT("name_contains"), TEXT("string"), true, TEXT("按名称包含过滤（当未提供targets时生效）")},
        {TEXT("class"), TEXT("string"), true, TEXT("按类名过滤（当未提供targets时生效）")},
        {TEXT("space"), TEXT("string"), true, TEXT("world 或 local，默认world")},
        {TEXT("translate"), TEXT("object"), true, TEXT("{x,y,z} cm 增量")},
        {TEXT("rotate"), TEXT("object"), true, TEXT("{pitch,yaw,roll} 度增量")},
        {TEXT("scale"), TEXT("object"), true, TEXT("{x,y,z} 缩放比例增量，如 0.1 表示 +10%")}
    };
    Spec.Permission = EToolPermission::Modify;
    Spec.bRequireConfirm = false;
}

FAIToolResult FTransformActorsDeltaTool::Execute(const TSharedPtr<FJsonObject>& Args)
{
    UWorld* World = SUEA::GetEditorWorld();
    if (!World) return {false, TEXT("未获取到编辑器世界"), nullptr};

    TArray<AActor*> Targets; if (!CollectTargetActors(World, Args, Targets))
    {
        return {false, TEXT("未找到目标Actor（请提供 targets，或选择一些Actor，或使用 name_contains/class 过滤）"), nullptr};
    }

    const FString Space = (Args.IsValid() && Args->HasTypedField<EJson::String>(TEXT("space"))) ? Args->GetStringField(TEXT("space")) : TEXT("world");

    FVector T(0); FRotator R(0); FVector S(0);
    if (Args.IsValid() && Args->HasTypedField<EJson::Object>(TEXT("translate")))
    {
        auto O = Args->GetObjectField(TEXT("translate"));
        if (O->HasTypedField<EJson::Number>(TEXT("x"))) T.X = O->GetNumberField(TEXT("x"));
        if (O->HasTypedField<EJson::Number>(TEXT("y"))) T.Y = O->GetNumberField(TEXT("y"));
        if (O->HasTypedField<EJson::Number>(TEXT("z"))) T.Z = O->GetNumberField(TEXT("z"));
    }
    if (Args.IsValid() && Args->HasTypedField<EJson::Object>(TEXT("rotate")))
    {
        auto O = Args->GetObjectField(TEXT("rotate"));
        if (O->HasTypedField<EJson::Number>(TEXT("pitch"))) R.Pitch = O->GetNumberField(TEXT("pitch"));
        if (O->HasTypedField<EJson::Number>(TEXT("yaw"))) R.Yaw = O->GetNumberField(TEXT("yaw"));
        if (O->HasTypedField<EJson::Number>(TEXT("roll"))) R.Roll = O->GetNumberField(TEXT("roll"));
    }
    if (Args.IsValid() && Args->HasTypedField<EJson::Object>(TEXT("scale")))
    {
        auto O = Args->GetObjectField(TEXT("scale"));
        if (O->HasTypedField<EJson::Number>(TEXT("x"))) S.X = O->GetNumberField(TEXT("x"));
        if (O->HasTypedField<EJson::Number>(TEXT("y"))) S.Y = O->GetNumberField(TEXT("y"));
        if (O->HasTypedField<EJson::Number>(TEXT("z"))) S.Z = O->GetNumberField(TEXT("z"));
    }

    const FScopedTransaction Tx(NSLOCTEXT("SmartUE", "TransformActorsDeltaTx", "AI: Transform Actors Delta"));
    int32 Affected = 0;

    for (AActor* A : Targets)
    {
        if (!IsValid(A)) continue;
        A->Modify();

        if (!T.IsNearlyZero())
        {
            if (Space.Equals(TEXT("local"), ESearchCase::IgnoreCase))
            {
                FVector NewLoc = A->GetActorLocation() + A->GetActorRotation().RotateVector(T);
                A->SetActorLocation(NewLoc, false, nullptr, ETeleportType::None);
            }
            else
            {
                A->AddActorWorldOffset(T, false, nullptr, ETeleportType::None);
            }
        }

        if (!R.IsNearlyZero())
        {
            if (Space.Equals(TEXT("local"), ESearchCase::IgnoreCase))
            {
                FRotator NewRot = (A->GetActorRotation().Quaternion() * R.Quaternion()).Rotator();
                A->SetActorRotation(NewRot, ETeleportType::None);
            }
            else
            {
                A->AddActorWorldRotation(R, false, nullptr, ETeleportType::None);
            }
        }

        if (!S.IsNearlyZero())
        {
            FVector Cur = A->GetActorScale3D();
            FVector NewS = Cur + S;
            A->SetActorScale3D(NewS);
        }

        ++Affected;
    }

    TSharedPtr<FJsonObject> Data = MakeShareable(new FJsonObject);
    Data->SetNumberField(TEXT("affected"), Affected);
    Data->SetStringField(TEXT("space"), Space);
    return {true, FString::Printf(TEXT("已对 %d 个Actor应用相对变换"), Affected), Data};
}

// ✅ 自动注册所有 Actor 工具
#include "ToolAutoRegister.h"

REGISTER_EDITOR_TOOL(FSelectAndFocusActorTool)
REGISTER_EDITOR_TOOL(FSetActorTransformTool)
REGISTER_EDITOR_TOOL(FCreateActorBasicTool)
REGISTER_EDITOR_TOOL(FDeleteActorTool)
REGISTER_EDITOR_TOOL(FTransformActorsDeltaTool)