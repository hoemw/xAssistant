// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

/**
 * 所有内置编辑器 AI 工具的聚合头文件
 * Aggregated header for all built-in editor AI tools
 * 
 * 此头文件为所有工具类别提供单一包含点：
 * This header provides a single include point for all tool categories:
 * - Actor 操作（创建、变换、删除）/ Actor manipulation (creation, transformation, deletion)
 * - 批量操作（重命名、可见性、标签、分布）/ Batch operations (rename, visibility, tags, distribution)
 * - 相机书签（保存、加载、管理视点）/ Camera bookmarks (save, load, manage viewpoints)
 * - 场景分析（统计、验证、优化建议）/ Scene analysis (statistics, validation, optimization suggestions)
 * - 选择工具（按名称、类、标签、模式）/ Selection tools (by name, class, tag, pattern)
 * - 查询工具（Actor 属性、列表、预设）/ Query tools (actor properties, lists, presets)
 * - 视口工具（聚焦、导航）/ Viewport tools (focus, navigation)
 * - 系统工具（控制台命令、保存、PIE 控制）/ System tools (console commands, save, PIE control)
 * - 灯光工具（属性修改）/ Lighting tools (property modification)
 */

#include "Tools/ActorTools.h"
#include "Tools/BatchOperationTools.h"
#include "Tools/CameraBookmarkTools.h"
#include "Tools/SceneAnalysisTools.h"
#include "Tools/PropertyManipulationTools.h"
#include "Tools/DebugTools.h"
#include "Tools/QuickLightTools.h"
#include "Tools/UniversalModifyTool.h"
#include "Tools/ViewportTools.h"
#include "Tools/QueryTools.h"
#include "Tools/SelectionTools.h"
#include "Tools/SystemTools.h"
#include "Tools/LightingTools.h"
