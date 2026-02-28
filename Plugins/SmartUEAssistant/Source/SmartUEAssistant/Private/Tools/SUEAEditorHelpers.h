// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Misc/Paths.h"
#include "Editor.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"

/**
 * xAssistant editor helper functions namespace
 */
namespace SUEA
{
	/**
	 * Get the current editor world
	 * @return Editor world pointer, or nullptr if not in editor
	 */
	inline UWorld* GetEditorWorld()
	{
#if WITH_EDITOR
		if (GEditor)
		{
			return GEditor->GetEditorWorldContext().World();
		}
#endif
		return nullptr;
	}

	/**
	 * Get the path to selection presets file
	 * @return File path for storing selection presets
	 */
	inline FString GetSelectionPresetPath()
	{
		return FPaths::ProjectSavedDir() / TEXT("SmartUEAssistant/SelectionPresets.json");
	}

	/**
	 * Load selection presets from disk
	 * @param OutObj Output JSON object containing presets
	 * @return True if load succeeded
	 */
	inline bool LoadSelectionPresets(TSharedPtr<FJsonObject>& OutObj)
	{
		const FString Path = GetSelectionPresetPath();
		if (!FPaths::FileExists(Path))
		{
			OutObj = MakeShareable(new FJsonObject);
			OutObj->SetObjectField(TEXT("presets"), MakeShareable(new FJsonObject));
			return true;
		}

		FString Text;
		if (!FFileHelper::LoadFileToString(Text, *Path))
		{
			return false;
		}

		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Text);
		TSharedPtr<FJsonObject> Root;
		if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
		{
			return false;
		}

		OutObj = Root;
		return true;
	}
}