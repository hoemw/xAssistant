// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorAIToolTypes.h"
#include "Dom/JsonObject.h"

/**
 * 调试工具 - 直接测试灯光颜色设置
 * Debug tool to test light color setting directly
 * 
 * 此工具绕过选择系统直接测试属性修改
 * This tool bypasses the selection system to directly test property modification
 */
class FDebugSetLightColorTool : public IEditorAITool
{
public:
	FDebugSetLightColorTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 调试工具 - 列出场景中的所有灯光组件
 * Debug tool to list all light components in the scene
 */
class FDebugListLightsTool : public IEditorAITool
{
public:
	FDebugListLightsTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 调试工具 - 显示选中 Actor 上可用的属性
 * Debug tool to show what properties are available on selected actors
 */
class FDebugShowPropertiesTool : public IEditorAITool
{
public:
	FDebugShowPropertiesTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

