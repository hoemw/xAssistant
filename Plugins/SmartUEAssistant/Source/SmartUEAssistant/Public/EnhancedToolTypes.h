// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorAIToolTypes.h"

/**
 * 增强工具类型定义（带扩展元数据）
 * Enhanced tool type definitions with extended metadata
 * 
 * 本文件扩展了 EditorAIToolTypes.h，增加了以下特性：
 * This file extends EditorAIToolTypes.h with additional features:
 * - 工具分类以便更好地组织 / Tool categories for better organization
 * - 版本管理 / Version management
 * - 弃用支持 / Deprecation support
 * - 增强的错误处理 / Enhanced error handling
 */

/**
 * 工具分类枚举
 * Tool category enumeration
 * 
 * 按功能区域分组工具，以便更好地发现
 * Groups tools by functional area for better discovery
 */
UENUM(BlueprintType)
enum class EToolCategory : uint8
{
	/** Actor 操作（创建、变换、删除） / Actor manipulation (creation, transformation, deletion) */
	Actor = 0,
	
	/** 选择和过滤 / Selection and filtering */
	Selection = 1,
	
	/** 视口和相机控制 / Viewport and camera control */
	Viewport = 2,
	
	/** 系统操作（保存、PIE 控制、控制台命令） / System operations (save, PIE control, console commands) */
	System = 3,
	
	/** 场景分析和验证 / Scene analysis and validation */
	Scene = 4,
	
	/** 属性操作 / Property manipulation */
	Property = 5,
	
	/** 批量操作 / Batch operations */
	Batch = 6,
	
	/** 相机书签 / Camera bookmarks */
	Camera = 7,
	
	/** 光照工具 / Lighting tools */
	Lighting = 8,
	
	/** 调试工具（仅开发使用） / Debug utilities (development only) */
	Debug = 9,
	
	/** 通用/泛用工具 / Universal/Generic tools */
	Universal = 10
};

/**
 * 工具执行的详细错误码
 * Detailed error codes for tool execution
 * 
 * 提供机器可读的错误分类
 * Provides machine-readable error classification
 * 
 * 注意：由于 uint16 限制，不向蓝图公开
 * Note: Not exposed to Blueprint due to uint16 limitation
 */
UENUM()
enum class EToolErrorCode : uint16
{
	/** 操作成功 / Operation succeeded */
	Success = 0,
	
	// 参数错误 (1xx) / Parameter errors (1xx)
	/** 参数无效或格式错误 / Invalid or malformed argument */
	InvalidArgument = 100,
	/** 缺少必需参数 / Required parameter missing */
	MissingRequiredParam = 101,
	/** 参数类型不匹配 / Parameter type mismatch */
	InvalidParamType = 102,
	/** 参数值超出范围或无效 / Parameter value out of range or invalid */
	InvalidParamValue = 103,
	
	// 资源错误 (2xx) / Resource errors (2xx)
	/** 场景中找不到 Actor / Actor not found in scene */
	ActorNotFound = 200,
	/** Actor 上找不到组件 / Component not found on actor */
	ComponentNotFound = 201,
	/** 找不到属性或无法访问 / Property not found or not accessible */
	PropertyNotFound = 202,
	/** 找不到资产或未加载 / Asset not found or not loaded */
	AssetNotFound = 203,
	/** 找不到关卡或未加载 / Level not found or not loaded */
	LevelNotFound = 204,
	
	// 权限错误 (3xx) / Permission errors (3xx)
	/** 由于权限级别不允许操作 / Operation not allowed due to permission level */
	PermissionDenied = 300,
	/** 操作需要用户确认 / Operation requires user confirmation */
	RequireConfirmation = 301,
	
	// 状态错误 (4xx) / State errors (4xx)
	/** 编辑器或世界状态无效 / Invalid editor or world state */
	InvalidState = 400,
	/** 当前状态不允许操作 / Operation not allowed in current state */
	OperationNotAllowed = 401,
	/** 资源为只读 / Resource is read-only */
	ReadOnly = 402,
	
	// 内部错误 (5xx) / Internal errors (5xx)
	/** 内部工具错误 / Internal tool error */
	InternalError = 500,
	/** 功能未实现 / Feature not implemented */
	NotImplemented = 501,
	/** 未知错误 / Unknown error */
	UnknownError = 599
};

/**
 * 增强的工具规格（带元数据）
 * Enhanced tool specification with metadata
 * 
 * 扩展 FAIToolSpec 并添加额外字段
 * Extends FAIToolSpec with additional fields
 */
struct SMARTUEASSISTANT_API FEnhancedToolSpec : public FAIToolSpec
{
	/** 工具分类（用于组织） / Tool category for organization */
	EToolCategory Category = EToolCategory::Actor;
	
	/** 可搜索的标签 / Searchable tags */
	TArray<FString> Tags;
	
	/** 语义化版本字符串（例如 "1.2.3"） / Semantic version string (e.g., "1.2.3") */
	FString Version = TEXT("1.0.0");
	
	/** 此工具是否已弃用 / Whether this tool is deprecated */
	bool bDeprecated = false;
	
	/** 替代此工具的新工具名称（如果已弃用） / Name of tool that replaces this one (if deprecated) */
	FString DeprecatedBy;
	
	/** 所需的最低 UE 版本（例如 "5.3"） / Minimum UE version required (e.g., "5.3") */
	FString MinEngineVersion;
	
	/** 预期最大执行时间提示（毫秒，0 = 无限制） / Maximum execution time hint (milliseconds, 0 = unlimited) */
	int32 ExpectedExecutionTimeMs = 0;
	
	/** 此工具是否修改场景（用于脏标记跟踪） / Whether this tool modifies the scene (for dirty tracking) */
	bool bModifiesScene = false;
	
	/** 转换为基本的 FAIToolSpec / Convert to basic FAIToolSpec */
	FAIToolSpec ToBasicSpec() const
	{
		FAIToolSpec Basic;
		Basic.Name = Name;
		Basic.Description = Description;
		Basic.Params = Params;
		Basic.Permission = Permission;
		Basic.bRequireConfirm = bRequireConfirm;
		return Basic;
	}
};

/**
 * 增强的工具执行结果（带详细错误信息）
 * Enhanced tool result with detailed error information
 */
struct SMARTUEASSISTANT_API FEnhancedToolResult : public FAIToolResult
{
	/** 详细错误码 / Detailed error code */
	EToolErrorCode ErrorCode = EToolErrorCode::Success;
	
	/** 技术性错误详情（供开发者使用） / Technical error details (for developers) */
	FString ErrorDetails;
	
	/** 用户友好的错误修复建议 / User-friendly suggestion for fixing the error */
	FString Suggestion;
	
	/** 执行时间（毫秒） / Execution time in milliseconds */
	float ExecutionTimeMs = 0.0f;
	
	/** 操作影响的对象数量 / Number of objects affected by the operation */
	int32 AffectedObjectCount = 0;
	
	// ===== 工厂方法 / Factory Methods =====
	
	/** 创建成功结果 / Create success result */
	static FEnhancedToolResult Success(const FString& Message, const TSharedPtr<FJsonObject>& Data = nullptr)
	{
		FEnhancedToolResult Result;
		Result.bSuccess = true;
		Result.Message = Message;
		Result.Data = Data;
		Result.ErrorCode = EToolErrorCode::Success;
		return Result;
	}
	
	/** 创建错误结果 / Create error result */
	static FEnhancedToolResult Error(
		EToolErrorCode Code,
		const FString& Message,
		const FString& Details = TEXT(""),
		const FString& InSuggestion = TEXT(""))
	{
		FEnhancedToolResult Result;
		Result.bSuccess = false;
		Result.Message = Message;
		Result.ErrorCode = Code;
		Result.ErrorDetails = Details;
		Result.Suggestion = InSuggestion;
		return Result;
	}
	
	/** 创建"未找到"错误 / Create "not found" error */
	static FEnhancedToolResult NotFound(const FString& ResourceType, const FString& ResourceName)
	{
		return Error(
			EToolErrorCode::ActorNotFound,
			FString::Printf(TEXT("%s未找到：%s"), *ResourceType, *ResourceName),
			FString::Printf(TEXT("Searched for %s with identifier '%s'"), *ResourceType, *ResourceName),
			TEXT("请检查名称拼写或使用列表工具查看可用对象")
		);
	}
	
	/** 创建无效参数错误 / Create invalid parameter error */
	static FEnhancedToolResult InvalidParam(
		const FString& ParamName,
		const FString& Reason,
		const FString& ExpectedFormat = TEXT(""))
	{
		FString Suggestion = ExpectedFormat.IsEmpty()
			? TEXT("请检查参数格式和取值范围")
			: FString::Printf(TEXT("期望格式：%s"), *ExpectedFormat);
		
		return Error(
			EToolErrorCode::InvalidParamValue,
			FString::Printf(TEXT("参数 '%s' 无效：%s"), *ParamName, *Reason),
			Reason,
			Suggestion
		);
	}
	
	/** 转换为基本的 FAIToolResult / Convert to basic FAIToolResult */
	FAIToolResult ToBasicResult() const
	{
		FAIToolResult Basic;
		Basic.bSuccess = bSuccess;
		Basic.Message = Message;
		Basic.Data = Data;
		return Basic;
	}
};

/**
 * 增强的工具接口（支持元数据）
 * Enhanced tool interface with metadata support
 * 
 * 继承自 IEditorAITool 并添加增强规格支持
 * Inherits from IEditorAITool and adds enhanced spec support
 */
class SMARTUEASSISTANT_API IEnhancedEditorTool : public IEditorAITool
{
public:
	virtual ~IEnhancedEditorTool() = default;
	
	/** 获取增强规格（带元数据） / Get enhanced specification with metadata */
	virtual const FEnhancedToolSpec& GetEnhancedSpec() const = 0;
	
	/** 执行并返回增强结果 / Execute with enhanced result */
	virtual FEnhancedToolResult ExecuteEnhanced(const TSharedPtr<FJsonObject>& Args) = 0;
	
	// 实现基础接口 / Implement base interface
	virtual const FAIToolSpec& GetSpec() const override
	{
		return GetEnhancedSpec();
	}
	
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override
	{
		return ExecuteEnhanced(Args).ToBasicResult();
	}
};

