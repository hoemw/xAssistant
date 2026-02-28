/**
 * 系统相关 AI 工具声明
 * System-related AI tools declarations
 */
#pragma once

#include "CoreMinimal.h"
#include "EditorAIToolTypes.h"
#include "Dom/JsonObject.h"

/**
 * 运行控制台命令工具
 * Run console command tool
 */
class FRunConsoleCommandTool : public IEditorAITool
{
public:
    FRunConsoleCommandTool();
    virtual const FAIToolSpec& GetSpec() const override { return Spec; }
    virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;
private:
    FAIToolSpec Spec;
};

/**
 * 保存关卡工具
 * Save level tool
 */
class FSaveLevelTool : public IEditorAITool
{
public:
    FSaveLevelTool();
    virtual const FAIToolSpec& GetSpec() const override { return Spec; }
    virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;
private:
    FAIToolSpec Spec;
};

/**
 * PIE（在编辑器中运行）控制工具
 * PIE (Play In Editor) control tool
 */
class FPIEControlTool : public IEditorAITool
{
public:
    FPIEControlTool();
    virtual const FAIToolSpec& GetSpec() const override { return Spec; }
    virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;
private:
    FAIToolSpec Spec;
};