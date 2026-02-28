/**
 * 查询相关 AI 工具声明
 * Query-related AI tools declarations
 */
#pragma once

#include "CoreMinimal.h"
#include "EditorAIToolTypes.h"
#include "Dom/JsonObject.h"

/**
 * 列出当前关卡中的 Actor 工具
 * List actors in current level tool
 */
class FListActorsTool : public IEditorAITool
{
public:
    FListActorsTool();
    virtual const FAIToolSpec& GetSpec() const override { return Spec; }
    virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;
private:
    FAIToolSpec Spec;
};

/**
 * 获取指定 Actor 常用属性工具
 * Get actor properties tool
 */
class FGetActorPropertiesTool : public IEditorAITool
{
public:
    FGetActorPropertiesTool();
    virtual const FAIToolSpec& GetSpec() const override { return Spec; }
    virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;
private:
    FAIToolSpec Spec;
};

// 列出可用的选择规则预设
class FListSelectionPresetsTool : public IEditorAITool
{
public:
    FListSelectionPresetsTool();
    virtual const FAIToolSpec& GetSpec() const override { return Spec; }
    virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;
private:
    FAIToolSpec Spec;
};