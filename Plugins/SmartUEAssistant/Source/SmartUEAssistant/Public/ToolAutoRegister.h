// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorAIToolRegistry.h"

/**
 * 自动工具注册系统
 * Automatic tool registration system
 * 
 * 用法 / Usage:
 *   // 在工具实现文件（.cpp）末尾添加：
 *   // At the end of your tool implementation file (.cpp):
 *   REGISTER_EDITOR_TOOL(FMyCustomTool)
 * 
 * 该宏创建一个静态初始化器，在模块加载时自动注册工具，
 * 无需在 SmartUEAssistant.cpp 中手动注册。
 * 
 * This macro creates a static initializer that automatically registers
 * the tool when the module loads, eliminating the need for manual
 * registration in SmartUEAssistant.cpp
 */

// 工具自动注册宏
// Tool auto-registration macro
#define REGISTER_EDITOR_TOOL(ToolClass) \
	namespace { \
		struct F##ToolClass##AutoRegister { \
			F##ToolClass##AutoRegister() { \
				FEditorAIToolRegistry::Get().Register(MakeShared<ToolClass>()); \
			} \
			~F##ToolClass##AutoRegister() { \
				/* 析构时注销（可选，注册表会自动清空） */ \
				/* Unregister on shutdown - optional, registry will be cleared anyway */ \
			} \
		}; \
		static F##ToolClass##AutoRegister G##ToolClass##Instance; \
	}

/**
 * 条件注册 - 仅在开发/调试版本中注册
 * Conditional registration - only registers in Development/Debug builds
 * 
 * 用法 / Usage:
 *   REGISTER_EDITOR_TOOL_DEBUG(FDebugTool)
 * 
 * Debug 工具将自动在 Shipping 版本中排除
 * Debug tools will be automatically excluded from Shipping builds
 */
#define REGISTER_EDITOR_TOOL_DEBUG(ToolClass) \
	namespace { \
		struct F##ToolClass##AutoRegister { \
			F##ToolClass##AutoRegister() { \
				IF_UE_BUILD_DEVELOPMENT_OR_DEBUG( \
					FEditorAIToolRegistry::Get().Register(MakeShared<ToolClass>()); \
				) \
			} \
		}; \
		static F##ToolClass##AutoRegister G##ToolClass##Instance; \
	}

// 构建类型检查辅助宏
// Helper macro for build type check
#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
	#define IF_UE_BUILD_DEVELOPMENT_OR_DEBUG(Code) Code
#else
	#define IF_UE_BUILD_DEVELOPMENT_OR_DEBUG(Code)
#endif

