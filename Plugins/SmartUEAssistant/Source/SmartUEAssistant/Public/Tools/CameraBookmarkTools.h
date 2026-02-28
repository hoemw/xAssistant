// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorAIToolTypes.h"
#include "Dom/JsonObject.h"

/**
 * 将当前视口相机位置保存为书签
 * Save current viewport camera position as a bookmark
 * 
 * 参数 / Parameters:
 * - Name (string): 书签名称 / Bookmark name
 * - Description (string, 可选): 书签描述 / Bookmark description
 */
class FSaveCameraBookmarkTool : public IEditorAITool
{
public:
	FSaveCameraBookmarkTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 跳转到已保存的相机书签
 * Jump to a saved camera bookmark
 * 
 * 参数 / Parameters:
 * - Name (string): 书签名称 / Bookmark name
 */
class FJumpToCameraBookmarkTool : public IEditorAITool
{
public:
	FJumpToCameraBookmarkTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 列出所有已保存的相机书签
 * List all saved camera bookmarks
 */
class FListCameraBookmarksTool : public IEditorAITool
{
public:
	FListCameraBookmarksTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

/**
 * 删除相机书签
 * Delete a camera bookmark
 * 
 * 参数 / Parameters:
 * - Name (string): 书签名称 / Bookmark name
 */
class FDeleteCameraBookmarkTool : public IEditorAITool
{
public:
	FDeleteCameraBookmarkTool();
	virtual const FAIToolSpec& GetSpec() const override { return Spec; }
	virtual FAIToolResult Execute(const TSharedPtr<FJsonObject>& Args) override;

private:
	FAIToolSpec Spec;
};

