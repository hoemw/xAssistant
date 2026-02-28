// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorAIToolTypes.h"
#include "Dom/JsonObject.h"

/**
 * 分析当前关卡统计信息
 * Analyze current level statistics
 * 
 * 返回 / Returns:
 * - 按类别的 Actor 计数 / Actor counts by class
 * - 总顶点/三角形数 / Total vertices/triangles
 * - 内存使用估算 / Memory usage estimates
 * - 灯光信息 / Lighting information
 */
class FAnalyzeLevelStatsTool : public IEditorAITool
{
public:
	FAnalyzeLevelStatsTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 查找关卡中缺失的资产引用
 * Find missing asset references in the level
 * 
 * 返回具有缺失引用的 Actor 列表
 * Returns list of actors with missing references
 */
class FFindMissingReferencesTool : public IEditorAITool
{
public:
	FFindMissingReferencesTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 查找关卡中重复的 Actor 名称
 * Find duplicate actor names in the level
 * 
 * 返回重复名称及其计数的列表
 * Returns list of duplicate names and their counts
 */
class FFindDuplicateNamesTool : public IEditorAITool
{
public:
	FFindDuplicateNamesTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 查找可能影响性能的超大网格
 * Find oversized meshes that may impact performance
 * 
 * 参数 / Parameters:
 * - VertexThreshold (number, 可选): 标记的最小顶点数（默认：50000）/ Minimum vertex count to flag (default: 50000)
 * 
 * 返回具有高多边形网格的 Actor 列表
 * Returns list of actors with high-poly meshes
 */
class FFindOversizedMeshesTool : public IEditorAITool
{
public:
	FFindOversizedMeshesTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 验证关卡是否存在常见问题
 * Validate level for common issues
 * 
 * 检查 / Checks for:
 * - 缺失碰撞 / Missing collision
 * - 不当的移动性设置 / Improper mobility settings
 * - 未点亮的 Actor / Unlit actors
 * - 超出世界边界的 Actor / Actors outside world bounds
 * 
 * 返回综合验证报告
 * Returns comprehensive validation report
 */
class FValidateLevelTool : public IEditorAITool
{
public:
	FValidateLevelTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

