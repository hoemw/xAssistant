/**
 * 灯光相关 AI 工具声明
 * Lighting-related AI tools declarations
 */
#pragma once

#include "CoreMinimal.h"
#include "EditorAIToolTypes.h"
#include "Dom/JsonObject.h"

/**
 * 设置光源属性工具（强度/颜色/色温），支持 set/add/mul
 * Set light property tool (intensity/color/temperature), supports set/add/mul
 */
class FSetLightPropertyTool : public IEditorAITool
{
public:
    FSetLightPropertyTool();
    virtual const FAIToolSpec& GetSpec() const override { return Spec; }
    virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;
private:
    FAIToolSpec Spec;
};