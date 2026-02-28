// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorAIToolTypes.h"
#include "Dom/JsonObject.h"

/**
 * 通用修改工具 - 一个工具修改任何对象的任何属性
 * Universal modification tool - one tool to modify any property on any object
 * 
 * 此工具将选择和修改合并为单个调用：
 * This tool combines selection and modification into a single call:
 * - 自动查找目标对象 / Automatically finds target objects
 * - 智能识别要修改的属性 / Intelligently identifies the property to modify
 * - 使用正确的 UE 通知应用更改 / Applies changes with proper UE notifications
 * 
 * 示例 / Examples:
 * {"Target":"灯光", "Property":"颜色", "Value":"红色"}
 * {"Target":"Cube", "Property":"位置", "Value":{"X":100,"Y":200,"Z":300}}
 * {"Target":"所有灯光", "Property":"亮度", "Value":5000}
 */
class FUniversalModifyTool : public IEditorAITool
{
public:
	FUniversalModifyTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
	
	/** 根据描述查找目标 Actor / Find target actors based on description */
	TArray<AActor*> FindTargetActors(const FString& TargetDescription);
	
	/** 根据属性提示查找属性路径 / Find property path based on property hint */
	FString FindPropertyPath(UObject* Object, const FString& PropertyHint);
	
	/** 对 Actor 应用修改 / Apply modification to an actor */
	bool ModifyActorProperty(AActor* Actor, const FString& PropertyPath, const TSharedPtr<FJsonValue>& Value, FString& OutError);
};

