#include "Tools/LightingTools.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "JsonObjectConverter.h"

// Editor-only includes
#include "Editor.h"
#include "ScopedTransaction.h"
#include "Engine/Selection.h"
#include "Engine/PointLight.h"
#include "Engine/DirectionalLight.h"
#include "Components/LightComponent.h"
#include "Tools/SUEAEditorHelpers.h"


// ---------------- FSetLightPropertyTool ----------------
FSetLightPropertyTool::FSetLightPropertyTool()
{
    Spec.Name = TEXT("set_light_property");
    Spec.Description = TEXT("设置光源属性：支持 intensity/color/temperature，操作 set|add|mul；目标可为名称或当前选择");
    Spec.Params = {
        {TEXT("name"), TEXT("string"), true, TEXT("目标Actor名称（精确匹配）。若省略则使用当前选择")},
        {TEXT("property"), TEXT("string"), false, TEXT("属性键：intensity | color | temperature")},
        {TEXT("op"), TEXT("string"), true, TEXT("操作：set | add | mul（color 仅支持 set）")},
        {TEXT("value"), TEXT("number"), true, TEXT("当属性为 intensity/temperature 时的数值")},
        {TEXT("color"), TEXT("object|string"), true, TEXT("当属性为 color 时的值，可为 {r,g,b[,a]} 或 #RRGGBB/#RRGGBBAA 或 'red' 等基本色名")}
    };
    Spec.Permission = EToolPermission::Modify;
    Spec.bRequireConfirm = false;
}

static bool ParseColorValue(const TSharedPtr<FJsonObject>& Args, FLinearColor& OutColor)
{
    if (!Args.IsValid()) return false;
    // 对象 {r,g,b[,a]}
    if (Args->HasTypedField<EJson::Object>(TEXT("color")))
    {
        auto Obj = Args->GetObjectField(TEXT("color"));
        double R=1, G=1, B=1, A=1;
        if (Obj->HasTypedField<EJson::Number>(TEXT("r"))) R = Obj->GetNumberField(TEXT("r"));
        if (Obj->HasTypedField<EJson::Number>(TEXT("g"))) G = Obj->GetNumberField(TEXT("g"));
        if (Obj->HasTypedField<EJson::Number>(TEXT("b"))) B = Obj->GetNumberField(TEXT("b"));
        if (Obj->HasTypedField<EJson::Number>(TEXT("a"))) A = Obj->GetNumberField(TEXT("a"));
        OutColor = FLinearColor((float)R, (float)G, (float)B, (float)A);
        return true;
    }
    // 字符串 #RRGGBB / #RRGGBBAA 或基本色名
    if (Args->HasTypedField<EJson::String>(TEXT("color")))
    {
        FString S = Args->GetStringField(TEXT("color"));
        S.TrimStartAndEndInline();
        if (S.StartsWith(TEXT("#")))
        {
            // 兼容UE5.6：使用FColor::FromHex解析，支持#RRGGBB或#RRGGBBAA
            FString Hex = S.RightChop(1);
            if (Hex.Len() == 6 || Hex.Len() == 8)
            {
                const FColor Parsed = FColor::FromHex(Hex);
                OutColor = FLinearColor(Parsed);
                return true;
            }
        }
        // 基本色名（小集合）
        static TMap<FString, FLinearColor> Named{
            {TEXT("white"), FLinearColor::White}, {TEXT("black"), FLinearColor::Black}, {TEXT("red"), FLinearColor::Red},
            {TEXT("green"), FLinearColor::Green}, {TEXT("blue"), FLinearColor::Blue}, {TEXT("yellow"), FLinearColor::Yellow},
            {TEXT("cyan"), FLinearColor(0.f,1.f,1.f,1.f)}, {TEXT("magenta"), FLinearColor(1.f,0.f,1.f,1.f)}
        };
        if (const FLinearColor* Found = Named.Find(S.ToLower())) { OutColor = *Found; return true; }
    }
    return false;
}

static void CollectLightComponentsFromActor(AActor* Actor, TArray<ULightComponentBase*>& OutComps)
{
    if (!IsValid(Actor)) return;
    // 如果是 ALight，优先使用其 LightComponent
    if (ALight* L = Cast<ALight>(Actor))
    {
        if (ULightComponent* LC = L->GetLightComponent())
        {
            OutComps.Add(LC);
            return;
        }
    }
    // 其它情况，搜索所有 LightComponentBase
    TArray<UActorComponent*> Comps; Actor->GetComponents(Comps);
    for (UActorComponent* C : Comps)
    {
        if (ULightComponentBase* LCB = Cast<ULightComponentBase>(C)) { OutComps.Add(LCB); }
    }
}

FAIToolResult FSetLightPropertyTool::Execute(const TSharedPtr<FJsonObject>& Args)
{
    if (!GEditor) return {false, TEXT("GEditor 不可用"), nullptr};
    UWorld* World = SUEA::GetEditorWorld();
    if (!World) return {false, TEXT("未获取到编辑器世界"), nullptr};

    if (!Args.IsValid() || !Args->HasTypedField<EJson::String>(TEXT("property")))
    {
        return {false, TEXT("缺少参数：property (intensity|color|temperature)"), nullptr};
    }
    const FString Prop = Args->GetStringField(TEXT("property"));
    const FString Op = (Args->HasTypedField<EJson::String>(TEXT("op"))) ? Args->GetStringField(TEXT("op")) : TEXT("set");

    // 收集目标Actor：name 精确匹配 或 当前选择集
    TArray<AActor*> Targets;
    if (Args->HasTypedField<EJson::String>(TEXT("name")))
    {
        const FString Name = Args->GetStringField(TEXT("name"));
        for (TActorIterator<AActor> It(World); It; ++It)
        {
            if ((*It)->GetName().Equals(Name, ESearchCase::IgnoreCase)) { Targets.Add(*It); break; }
        }
        if (Targets.Num() == 0) return {false, FString::Printf(TEXT("未找到Actor：%s"), *Name), nullptr};
    }
    else
    {
        if (USelection* Sel = GEditor->GetSelectedActors())
        {
            for (FSelectionIterator It(*Sel); It; ++It)
            {
                if (AActor* A = Cast<AActor>(*It)) Targets.Add(A);
            }
        }
        if (Targets.Num() == 0) return {false, TEXT("未提供 name 且当前无选择，请先选择光源Actor或提供名称"), nullptr};
    }

    // 收集所有Light组件
    TArray<ULightComponentBase*> LightComps;
    for (AActor* A : Targets) { CollectLightComponentsFromActor(A, LightComps); }
    if (LightComps.Num() == 0) return {false, TEXT("目标中未找到任何光源组件"), nullptr};

    const FScopedTransaction Tx(NSLOCTEXT("SmartUE", "SetLightPropertyTx", "AI: Set Light Property"));

    int32 Affected = 0;
    FString Msg;

    if (Prop.Equals(TEXT("intensity"), ESearchCase::IgnoreCase))
    {
        if (!Args->HasTypedField<EJson::Number>(TEXT("value")))
            return {false, TEXT("intensity 需要参数 value:number"), nullptr};
        const float V = (float)Args->GetNumberField(TEXT("value"));
        for (ULightComponentBase* B : LightComps)
        {
            if (ULightComponent* L = Cast<ULightComponent>(B))
            {
                L->Modify();
                float NewVal = V;
                if (Op.Equals(TEXT("add"), ESearchCase::IgnoreCase)) NewVal = L->Intensity + V;
                else if (Op.Equals(TEXT("mul"), ESearchCase::IgnoreCase)) NewVal = L->Intensity * V;
                L->SetIntensity(NewVal);
                L->MarkRenderStateDirty();
                ++Affected;
            }
        }
        Msg = FString::Printf(TEXT("已对 %d 个光源设置强度（op=%s）"), Affected, *Op);
    }
    else if (Prop.Equals(TEXT("temperature"), ESearchCase::IgnoreCase))
    {
        if (!Args->HasTypedField<EJson::Number>(TEXT("value")))
            return {false, TEXT("temperature 需要参数 value:number"), nullptr};
        const float V = (float)Args->GetNumberField(TEXT("value"));
        for (ULightComponentBase* B : LightComps)
        {
            if (ULightComponent* L = Cast<ULightComponent>(B))
            {
                L->Modify();
                float NewVal = V;
                if (Op.Equals(TEXT("add"), ESearchCase::IgnoreCase)) NewVal = L->Temperature + V;
                else if (Op.Equals(TEXT("mul"), ESearchCase::IgnoreCase)) NewVal = L->Temperature * V;
                L->SetTemperature(NewVal);
                L->bUseTemperature = true;
                L->MarkRenderStateDirty();
                ++Affected;
            }
        }
        Msg = FString::Printf(TEXT("已对 %d 个光源设置色温（op=%s）"), Affected, *Op);
    }
    else if (Prop.Equals(TEXT("color"), ESearchCase::IgnoreCase))
    {
        FLinearColor C(1,1,1,1);
        if (!ParseColorValue(Args, C))
            return {false, TEXT("color 需要参数 color:{r,g,b[,a]} 或 字符串 #RRGGBB"), nullptr};
        for (ULightComponentBase* B : LightComps)
        {
            if (ULightComponent* L = Cast<ULightComponent>(B))
            {
                L->Modify();
                L->SetLightColor(C, true);
                L->MarkRenderStateDirty();
                ++Affected;
            }
        }
        Msg = FString::Printf(TEXT("已对 %d 个光源设置颜色"), Affected);
    }
    else
    {
        return {false, TEXT("不支持的 property，允许：intensity/color/temperature"), nullptr};
    }

    if (GEditor)
    {
        GEditor->RedrawLevelEditingViewports(true);
        GEditor->NoteSelectionChange();
    }

    TSharedPtr<FJsonObject> Data2 = MakeShareable(new FJsonObject);
    Data2->SetNumberField(TEXT("affected"), Affected);
    Data2->SetStringField(TEXT("property"), Prop);
    Data2->SetStringField(TEXT("op"), Op);
    return {Affected > 0, Msg, Data2};
}
// �?自动注册工具
#include "ToolAutoRegister.h"

REGISTER_EDITOR_TOOL(FSetLightPropertyTool)

