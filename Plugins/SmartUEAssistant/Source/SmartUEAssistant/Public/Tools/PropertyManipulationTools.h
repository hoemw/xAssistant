// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorAIToolTypes.h"
#include "Dom/JsonObject.h"

/**
 * 为选中的 Actor 设置属性值
 * Set property value for selected actors
 * 
 * 这是一个通用属性设置器，使用反射系统工作。
 * 支持常见属性类型：float、int、bool、FVector、FRotator、FLinearColor、FColor
 * 
 * This is a universal property setter that works with reflection system.
 * Supports common property types: float, int, bool, FVector, FRotator, FLinearColor, FColor
 * 
 * 参数 / Parameters:
 * - PropertyPath (string): 属性路径（例如 "LightComponent.LightColor"、"Intensity"）/ Property path
 * - Value (mixed): 属性值（类型取决于属性）/ Property value (type depends on property)
 * - ApplyToComponents (boolean, 可选): 也在组件中搜索 / Search in components too
 */
class FSetActorPropertyTool : public IEditorAITool
{
public:
	FSetActorPropertyTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

	/** 供其他工具使用的公共辅助方法 / Public helper for other tools to use */
	bool SetPropertyValue(UObject* Object, const FString& PropertyPath, const TSharedPtr<FJsonValue>& Value, FString& OutError);

private:
	FAIToolSpec Spec;
};

/**
 * 批量设置选中 Actor 的属性
 * Batch set properties for selected actors
 * 
 * 在一次调用中设置多个属性以提高效率
 * Set multiple properties in one call for efficiency
 * 
 * 参数 / Parameters:
 * - Properties (object): 属性路径到值的映射 / Map of property paths to values
 *   示例 / Example: {"LightColor": {"R":1.0,"G":0.0,"B":0.0}, "Intensity": 5000.0}
 */
class FBatchSetPropertiesTool : public IEditorAITool
{
public:
	FBatchSetPropertiesTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 获取 Actor 的可用属性
 * Get available properties for an actor
 * 
 * 发现选中 Actor 上可以修改的属性。
 * 对 AI 了解可用属性很有用。
 * 
 * Discover what properties can be modified on selected actors.
 * Useful for the AI to know what properties are available.
 * 
 * 参数 / Parameters:
 * - ActorName (string, 可选): 特定 Actor 名称，或使用首个选中的 / Specific actor name, or use first selected
 * - IncludeComponents (boolean, 可选): 包含组件属性 / Include component properties
 */
class FGetAvailablePropertiesTool : public IEditorAITool
{
public:
	FGetAvailablePropertiesTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
	
	void DiscoverProperties(UObject* Object, const FString& Prefix, TArray<FString>& OutProperties);
};

/**
 * 智能属性设置器 - 支持自然语言
 * Smart property setter with natural language support
 * 
 * 基于语义理解智能设置属性
 * Intelligently sets properties based on semantic understanding
 * 
 * 示例 / Examples:
 * - "color" -> 搜索 Color、LightColor 等 / searches for Color, LightColor, etc.
 * - "brightness" -> 搜索 Intensity、Brightness 等 / searches for Intensity, Brightness, etc.
 * - "size" -> 搜索 Scale、Radius 等 / searches for Scale, Radius, etc.
 * 
 * 参数 / Parameters:
 * - PropertyHint (string): 自然语言属性提示（例如 "color"、"brightness"）/ Natural language property hint
 * - Value (mixed): 属性值 / Property value
 */
class FSmartSetPropertyTool : public IEditorAITool
{
public:
	FSmartSetPropertyTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
	
	TArray<FString> FindMatchingProperties(UObject* Object, const FString& Hint);
	FString GetPropertyPath(UObject* Object, const FString& PropertyName);
};

/**
 * 按偏移量调整数值属性（相对变化）
 * Adjust numeric property by offset (relative change)
 * 
 * 用于"增加"、"减少"操作
 * Useful for "increase by", "decrease by" operations
 * 
 * 参数 / Parameters:
 * - PropertyPath (string): 属性路径 / Property path
 * - Offset (number): 要添加的量（可以为负）/ Amount to add (can be negative)
 * - Multiply (boolean, 可选): 如果为 true，执行乘法而非加法 / If true, multiply instead of add
 */
class FAdjustPropertyTool : public IEditorAITool
{
public:
	FAdjustPropertyTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 在 Actor 之间复制属性值
 * Copy property values between actors
 * 
 * 参数 / Parameters:
 * - SourceActor (string): 源 Actor 名称 / Source actor name
 * - PropertyPaths (array of strings): 要复制的属性 / Properties to copy
 * - ApplyToSelected (boolean): 应用到所有选中的 Actor / Apply to all selected actors
 */
class FCopyPropertiesTool : public IEditorAITool
{
public:
	FCopyPropertiesTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

