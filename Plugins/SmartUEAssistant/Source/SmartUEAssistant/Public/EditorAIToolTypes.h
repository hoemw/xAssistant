// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"

/**
 * 编辑器操作的权限级别
 * Permission level for editor operations
 * 
 * 用于控制危险操作并要求用户确认
 * Used to control dangerous operations and require user confirmation
 */
enum class EToolPermission : uint8
{
	/** 安全操作（选择、查询等）/ Safe operations (selection, queries, etc.) */
	Safe = 0,
	
	/** 修改场景（变换、添加组件等）/ Modify scene (transform, add components, etc.) */
	Modify = 1,
	
	/** 潜在危险操作（保存、运行命令等）/ Potentially dangerous operations (save, run commands, etc.) */
	Dangerous = 2
};

/**
 * AI 工具的参数规范
 * Parameter specification for AI tools
 */
struct FAIToolParam
{
	/** 参数名称 / Parameter name */
	FString Name;
	
	/** 参数类型："string" | "number" | "boolean" | "object" / Parameter type */
	FString Type;
	
	/** 此参数是否可选 / Whether this parameter is optional */
	bool bOptional = false;
	
	/** 参数的人类可读描述 / Human-readable description of the parameter */
	FString Description;
};

/**
 * 工具规范定义 - 定义工具接口和元数据
 * Tool specification defining tool interface and metadata
 */
struct FAIToolSpec
{
	/** 唯一的工具名称 / Unique tool name */
	FString Name;
	
	/** 人类可读的工具描述 / Human-readable tool description */
	FString Description;
	
	/** 参数规范数组 / Array of parameter specifications */
	TArray<FAIToolParam> Params;
	
	/** 此工具的权限级别 / Permission level for this tool */
	EToolPermission Permission = EToolPermission::Safe;
	
	/** 执行前是否需要明确的用户确认 / Whether explicit user confirmation is required before execution */
	bool bRequireConfirm = false;
};

/**
 * 工具执行结果
 * Result of tool execution
 */
struct FAIToolResult
{
	/** 执行是否成功 / Whether the execution succeeded */
	bool bSuccess = false;
	
	/** 结果消息或错误描述 / Result message or error description */
	FString Message;
	
	/** 工具返回的可选结构化数据 / Optional structured data returned by the tool */
	TSharedPtr<FJsonObject> Data;
};

/**
 * 编辑器 AI 工具接口
 * Interface for editor AI tools
 * 
 * 实现此接口以创建可被 AI 助手调用的自定义工具。
 * 工具接收 JSON 参数并返回结构化结果。
 * 
 * Implement this interface to create custom tools that can be called by the AI assistant.
 * Tools receive JSON arguments and return structured results.
 */
class IEditorAITool
{
public:
	virtual ~IEditorAITool() = default;

	/**
	 * 获取工具规范
	 * Get the tool specification
	 * 
	 * @return 定义名称、描述、参数和权限的工具规范 / Tool spec defining name, description, parameters, and permissions
	 */
	virtual const FAIToolSpec& GetSpec() const = 0;

	/**
	 * 使用给定参数执行工具
	 * Execute the tool with given arguments
	 * 
	 * @param Args 包含工具参数的 JSON 对象 / JSON object containing tool arguments
	 * @return 包含成功状态和消息的执行结果 / Execution result with success status and message
	 */
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) = 0;
};