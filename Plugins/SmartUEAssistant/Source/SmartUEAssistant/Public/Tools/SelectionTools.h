/**
 * 选择相关 AI 工具声明
 * Selection-related AI tools declarations
 */
#pragma once

#include "CoreMinimal.h"
#include "EditorAIToolTypes.h"
#include "Dom/JsonObject.h"

/**
 * 按规则选择 Actor 工具（按类/名称/标签/组件/正则等，支持 set/add/remove/invert）
 * Select actors by rule tool (by class/name/tag/component/regex, supports set/add/remove/invert)
 */
class FSelectActorsByRuleTool : public IEditorAITool
{
public:
    FSelectActorsByRuleTool();
    virtual const FAIToolSpec& GetSpec() const override { return Spec; }
    virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;
private:
    FAIToolSpec Spec;
};