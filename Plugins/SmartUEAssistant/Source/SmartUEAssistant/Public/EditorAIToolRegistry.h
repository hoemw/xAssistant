// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorAIToolTypes.h"
#include "Dom/JsonObject.h"

/**
 * 编辑器 AI 工具的注册表和调度器
 * Registry and dispatcher for Editor AI Tools
 * 
 * 管理所有可用 AI 工具的单例注册表。
 * 工具可以在模块启动时注册，并通过名称调度。
 * 
 * Singleton registry that manages all available AI tools.
 * Tools can be registered at module startup and dispatched by name.
 * 
 * 用法 / Usage:
 *   FEditorAIToolRegistry::Get().Register(MakeShared<FMyCustomTool>());
 *   FAIToolResult Result = FEditorAIToolRegistry::Get().Dispatch(TEXT("my_tool"), Args);
 */
class FEditorAIToolRegistry
{
public:
	/**
	 * 获取单例实例
	 * Get the singleton instance
	 * 
	 * @return 全局工具注册表的引用 / Reference to the global tool registry
	 */
	static FEditorAIToolRegistry& Get()
	{
		static FEditorAIToolRegistry Instance;
		return Instance;
	}

	/**
	 * 注册新工具
	 * Register a new tool
	 * 
	 * @param Tool 工具实现的共享引用 / Shared reference to the tool implementation
	 */
	void Register(TSharedRef<IEditorAITool> Tool)
	{
		Tools.Add(Tool->GetSpec().Name, Tool);
	}

	/**
	 * 按名称注销工具
	 * Unregister a tool by name
	 * 
	 * @param Name 要移除的工具名称 / Name of the tool to remove
	 */
	void Unregister(const FString& Name)
	{
		Tools.Remove(Name);
	}

	/**
	 * 注销所有工具（用于模块关闭）
	 * Unregister all tools (for module shutdown)
	 */
	void UnregisterAll()
	{
		Tools.Empty();
	}

	/**
	 * 检查工具是否已注册
	 * Check if a tool is registered
	 * 
	 * @param Name 要检查的工具名称 / Tool name to check
	 * @return 如果工具存在于注册表中则返回 true / True if the tool exists in the registry
	 */
	bool HasTool(const FString& Name) const
	{
		return Tools.Contains(Name);
	}

	/**
	 * 获取所有已注册的工具
	 * Get all registered tools
	 * 
	 * @return 工具名称到工具实现的映射 / Map of tool names to tool implementations
	 */
	const TMap<FString, TSharedRef<IEditorAITool>>& GetAll() const
	{
		return Tools;
	}

	/**
	 * 按名称调度工具执行
	 * Dispatch a tool execution by name
	 * 
	 * @param Name 要执行的工具名称 / Tool name to execute
	 * @param Args 工具的 JSON 参数 / JSON arguments for the tool
	 * @return 执行结果 / Execution result
	 */
	FAIToolResult Dispatch(const FString& Name, const TSharedPtr<FJsonObject>& Args)
	{
		if (const TSharedRef<IEditorAITool>* Found = Tools.Find(Name))
		{
			return (*Found)->Execute(Args);
		}
		return {false, FString::Printf(TEXT("Tool not found: %s"), *Name), nullptr};
	}

private:
	/** 已注册工具的映射表 / Map of registered tools */
	TMap<FString, TSharedRef<IEditorAITool>> Tools;
};