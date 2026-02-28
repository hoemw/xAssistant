// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorAIToolTypes.h"
#include "Dom/JsonObject.h"

/**
 * 按名称选择并聚焦 Actor 的工具（模糊匹配）
 * Tool to select and focus on an actor by name (fuzzy matching)
 */
class FSelectAndFocusActorTool : public IEditorAITool
{
public:
	FSelectAndFocusActorTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 设置 Actor 变换的工具（位置/旋转/缩放）
 * Tool to set actor transform (location/rotation/scale)
 */
class FSetActorTransformTool : public IEditorAITool
{
public:
	FSetActorTransformTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 创建基础 Actor 的工具（空 Actor、灯光、基础体）
 * Tool to create basic actors (empty actor, lights, primitives)
 */
class FCreateActorBasicTool : public IEditorAITool
{
public:
	FCreateActorBasicTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * Tool to delete actors from the level
 */
class FDeleteActorTool : public IEditorAITool
{
public:
	FDeleteActorTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * Tool to apply relative transforms to multiple actors
 */
class FTransformActorsDeltaTool : public IEditorAITool
{
public:
	FTransformActorsDeltaTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};