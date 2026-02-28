// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorAIToolTypes.h"
#include "Dom/JsonObject.h"

/**
 * 批量重命名选中的 Actor，支持前缀、后缀和编号
 * Batch rename selected actors with prefix, suffix, and numbering support
 * 
 * 参数 / Parameters:
 * - Prefix (string, 可选): 在 Actor 名称前添加的文本 / Text to add before actor names
 * - Suffix (string, 可选): 在 Actor 名称后添加的文本 / Text to add after actor names
 * - StartIndex (number, 可选): 顺序编号的起始数字 / Starting number for sequential numbering
 * - RemovePrefix (string, 可选): 要从现有名称中移除的前缀 / Prefix to remove from existing names
 */
class FBatchRenameActorsTool : public IEditorAITool
{
public:
	FBatchRenameActorsTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 批量设置选中 Actor 的可见性
 * Batch set visibility for selected actors
 * 
 * 参数 / Parameters:
 * - Visible (boolean): Actor 是否应该可见 / Whether actors should be visible
 * - ApplyToChildren (boolean, 可选): 递归应用到所有子对象 / Apply to all children recursively
 */
class FBatchSetVisibilityTool : public IEditorAITool
{
public:
	FBatchSetVisibilityTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 批量设置选中 Actor 的移动性
 * Batch set mobility for selected actors
 * 
 * 参数 / Parameters:
 * - Mobility (string): "Static"、"Stationary" 或 "Movable" / "Static", "Stationary", or "Movable"
 */
class FBatchSetMobilityTool : public IEditorAITool
{
public:
	FBatchSetMobilityTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 批量将选中的 Actor 移动到指定关卡
 * Batch move selected actors to a specific level
 * 
 * 参数 / Parameters:
 * - LevelName (string): 目标关卡的名称 / Name of the target level
 */
class FBatchMoveToLevelTool : public IEditorAITool
{
public:
	FBatchMoveToLevelTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 批量设置 Actor 标签
 * Batch set actor tags
 * 
 * 参数 / Parameters:
 * - Tags (array of strings): 要设置的标签 / Tags to set
 * - Mode (string): "Set"、"Add" 或 "Remove" / "Set", "Add", or "Remove"
 */
class FBatchSetTagsTool : public IEditorAITool
{
public:
	FBatchSetTagsTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 将选中的 Actor 对齐到地面/表面
 * Align selected actors to ground/surface
 * 
 * 参数 / Parameters:
 * - AlignRotation (boolean, 可选): 将旋转对齐到表面法线 / Align rotation to surface normal
 * - Offset (number, 可选): 距离表面的 Z 轴偏移 / Z-axis offset from surface
 */
class FAlignToGroundTool : public IEditorAITool
{
public:
	FAlignToGroundTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 按模式分布选中的 Actor
 * Distribute selected actors in a pattern
 * 
 * 参数 / Parameters:
 * - Pattern (string): "Line"、"Grid"、"Circle" 或 "Random" / "Line", "Grid", "Circle", or "Random"
 * - Spacing (number): Actor 之间的距离 / Distance between actors
 * - Rows (number, 可选): 网格模式的行数 / For grid pattern
 * - Columns (number, 可选): 网格模式的列数 / For grid pattern
 * - Radius (number, 可选): 圆形模式的半径 / For circle pattern
 */
class FDistributeActorsTool : public IEditorAITool
{
public:
	FDistributeActorsTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

