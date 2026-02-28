#include "Tools/SystemTools.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "JsonObjectConverter.h"
#include "Editor.h"
#include "FileHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ScopedTransaction.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"
#include "Editor/EditorEngine.h"
#include "Tools/SUEAEditorHelpers.h"
#include "ConsoleCommandWhitelist.h"  // 新增：控制台命令安全白名单


// ---------------- FRunConsoleCommandTool ----------------
FRunConsoleCommandTool::FRunConsoleCommandTool()
{
    Spec.Name = TEXT("run_console_command");
    Spec.Description = TEXT("在编辑器中执行控制台命令（危险操作，需确认）");
    Spec.Params = {
        {TEXT("command"), TEXT("string"), false, TEXT("控制台命令字符串")}
    };
    Spec.Permission = EToolPermission::Dangerous;
    Spec.bRequireConfirm = true;
}

FAIToolResult FRunConsoleCommandTool::Execute(const TSharedPtr<FJsonObject>& Args)
{
    if (!GEditor) return {false, TEXT("GEditor 不可用"), nullptr};
    FString Cmd;
    if (Args.IsValid())
    {
        if (Args->HasTypedField<EJson::String>(TEXT("command")))
        {
            Cmd = Args->GetStringField(TEXT("command"));
        }
        else if (Args->HasTypedField<EJson::String>(TEXT("cmd")))
        {
            Cmd = Args->GetStringField(TEXT("cmd"));
        }
    }
    if (Cmd.IsEmpty())
    {
        return {false, TEXT("缺少参数：command（或 cmd）"), nullptr};
    }

    // ✅ 新增：控制台命令安全检查
    if (!FConsoleCommandWhitelist::IsCommandSafe(Cmd))
    {
        FString Reason = FConsoleCommandWhitelist::GetBlockReason(Cmd);
        return {false, FString::Printf(TEXT("命令被阻止：%s"), *Reason), nullptr};
    }

    UWorld* World = SUEA::GetEditorWorld();
    if (!World) return {false, TEXT("未获取到编辑器世界"), nullptr};
    if (GEditor->Exec(World, *Cmd))
    {
        return {true, FString::Printf(TEXT("已执行命令：%s"), *Cmd), nullptr};
    }
    return {false, FString::Printf(TEXT("执行失败：%s"), *Cmd), nullptr};
}

// ---------------- FSaveLevelTool ----------------
FSaveLevelTool::FSaveLevelTool()
{
    Spec.Name = TEXT("save_level");
    Spec.Description = TEXT("保存当前关卡与脏包（需要权限）");
    Spec.Params = {};
    Spec.Permission = EToolPermission::Dangerous;
    Spec.bRequireConfirm = true;
}

FAIToolResult FSaveLevelTool::Execute(const TSharedPtr<FJsonObject>& Args)
{
    if (!GEditor) return {false, TEXT("GEditor 不可用"), nullptr};
    const FScopedTransaction Tx(NSLOCTEXT("SmartUE", "SaveLevelTx", "AI: Save Level"));

    UWorld* World = SUEA::GetEditorWorld();
    if (!World)
    {
        return {false, TEXT("未获取到编辑器世界"), nullptr};
    }

    const bool bSuccess = FEditorFileUtils::SaveDirtyPackages(/*bPromptUserToSave*/ false, /*bSaveMapPackages*/ true, /*bSaveContentPackages*/ true, /*bFastSave*/ false, /*bNotifyNoPackagesSaved*/ false, /*bCanBeDeclined*/ true);

    return { bSuccess, bSuccess ? TEXT("已保存") : TEXT("保存失败或无改动/被取消"), nullptr };
}

// ---------------- FPIEControlTool ----------------
FPIEControlTool::FPIEControlTool()
{
    Spec.Name = TEXT("pie_control");
    Spec.Description = TEXT("PIE 运行控制：目前支持 stop 与状态查询");
    Spec.Params = {
        {TEXT("action"), TEXT("string"), false, TEXT("stop | status （start将于后续实现）")}
    };
    Spec.Permission = EToolPermission::Modify;
    Spec.bRequireConfirm = false;
}

FAIToolResult FPIEControlTool::Execute(const TSharedPtr<FJsonObject>& Args)
{
    if (!GEditor)
    {
        return { false, TEXT("GEditor 不可用"), nullptr };
    }

    const FString Action = Args->GetStringField(TEXT("action")); // "play" | "stop" | "pause" | "resume"

    if (Action.Equals(TEXT("play"), ESearchCase::IgnoreCase))
    {
        FRequestPlaySessionParams Params; // 使用默认：InProcess + PlayInEditor
        // 如需修改类型：Params.WorldType = EPlaySessionWorldType::PlayInEditor;
        GEditor->RequestPlaySession(Params);
        return { true, TEXT("已发起 PIE"), nullptr };
    }
    else if (Action.Equals(TEXT("stop"), ESearchCase::IgnoreCase))
    {
        GEditor->RequestEndPlayMap();
        return { true, TEXT("已停止 PIE"), nullptr };
    }
    else if (Action.Equals(TEXT("pause"), ESearchCase::IgnoreCase))
    {
        if (GEditor->PlayWorld)
        {
            GEditor->PlayWorld->bDebugPauseExecution = true;
            return { true, TEXT("PIE 已暂停"), nullptr };
        }
        return { false, TEXT("当前不在 PIE"), nullptr };
    }
    else if (Action.Equals(TEXT("resume"), ESearchCase::IgnoreCase))
    {
        if (GEditor->PlayWorld)
        {
            GEditor->PlayWorld->bDebugPauseExecution = false;
            return { true, TEXT("PIE 已继续"), nullptr };
        }
        return { false, TEXT("当前不在 PIE"), nullptr };
    }

    return { false, TEXT("未知 action"), nullptr };
}
// �?自动注册工具
#include "ToolAutoRegister.h"

REGISTER_EDITOR_TOOL(FRunConsoleCommandTool)
REGISTER_EDITOR_TOOL(FSaveLevelTool)
REGISTER_EDITOR_TOOL(FPIEControlTool)

