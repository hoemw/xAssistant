// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorAIToolTypes.h"
#include "Dom/JsonObject.h"

/**
 * 快速设置灯光颜色工具 - 绕过反射系统
 * Quick tool to set light color - bypasses reflection system
 * 
 * 作用于选中的灯光或场景中的所有灯光
 * Works on selected lights or all lights in scene
 */
class FQuickSetLightColorTool : public IEditorAITool
{
public:
	FQuickSetLightColorTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 快速设置灯光强度工具
 * Quick tool to set light intensity
 * 
 * 作用于选中的灯光或场景中的所有灯光
 * Works on selected lights or all lights in scene
 */
class FQuickSetLightIntensityTool : public IEditorAITool
{
public:
	FQuickSetLightIntensityTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

