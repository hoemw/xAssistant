#include "Tools/ViewportTools.h"
#include "Editor.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "JsonObjectConverter.h"
#include "Engine/Selection.h"
#include "Tools/SUEAEditorHelpers.h"


FFocusViewportTool::FFocusViewportTool()
{
    Spec.Name = TEXT("focus_viewport");
    Spec.Description = TEXT("将视口聚焦到目标Actor或当前选择");
    Spec.Params = {
        {TEXT("name"), TEXT("string"), true, TEXT("目标Actor名称（精确匹配）。若省略则使用当前选择")}
    };
    Spec.Permission = EToolPermission::Safe;
    Spec.bRequireConfirm = false;
}

FAIToolResult FFocusViewportTool::Execute(const TSharedPtr<FJsonObject>& Args)
{
    if (!GEditor) return {false, TEXT("GEditor 不可用"), nullptr};
    UWorld* World = SUEA::GetEditorWorld();
    if (!World) return {false, TEXT("未获取到编辑器世界"), nullptr};

    // 收集目标Actor：name 精确匹配 或 当前选择集
    TArray<AActor*> Targets;
    if (Args.IsValid() && Args->HasTypedField<EJson::String>(TEXT("name")))
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
        if (Targets.Num() == 0) return {false, TEXT("未提供 name 且当前无选择"), nullptr};
    }

    GEditor->MoveViewportCamerasToActor(Targets, /*bActiveViewportOnly*/ false);
    return {true, TEXT("视口已聚焦到目标"), nullptr};
}
// �?自动注册工具
#include "ToolAutoRegister.h"

REGISTER_EDITOR_TOOL(FFocusViewportTool)

