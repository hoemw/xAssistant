/**
 * 视口相关 AI 工具声明
 * Viewport-related AI tools declarations
 */
#pragma once

#include "CoreMinimal.h"
#include "EditorAIToolTypes.h"
#include "Dom/JsonObject.h"

/**
 * 聚焦视口到 Actor 或位置工具
 * Focus viewport to actor or location tool
 */
class FFocusViewportTool : public IEditorAITool
{
public:
    FFocusViewportTool();
    virtual const FAIToolSpec& GetSpec() const override { return Spec; }
    virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;
private:
    FAIToolSpec Spec;
};