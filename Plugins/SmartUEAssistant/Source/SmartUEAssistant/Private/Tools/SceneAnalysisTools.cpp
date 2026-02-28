// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tools/SceneAnalysisTools.h"
#include "SmartUEAssistantLog.h"
#include "Editor.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/Light.h"
#include "Components/StaticMeshComponent.h"
#include "Components/LightComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "AssetRegistry/AssetRegistryModule.h"

// ==================== Analyze Level Stats Tool ====================

FAnalyzeLevelStatsTool::FAnalyzeLevelStatsTool()
{
	Spec.Name = TEXT("analyze_level_stats");
	Spec.Description = TEXT("Analyze current level statistics including actor counts, vertices, and memory estimates");
	Spec.Permission = EToolPermission::Safe;
	Spec.bRequireConfirm = false;
}

FAIToolResult FAnalyzeLevelStatsTool::Execute(const TSharedPtr<FJsonObject>& Args)
{
	if (!GEditor)
	{
		return {false, TEXT("Editor not available"), nullptr};
	}

	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World)
	{
		return {false, TEXT("No world available"), nullptr};
	}

	// Count actors by class
	TMap<FString, int32> ActorCountsByClass;
	int32 TotalActors = 0;
	int64 TotalVertices = 0;
	int64 TotalTriangles = 0;
	int32 StaticActors = 0;
	int32 MovableActors = 0;
	int32 StationaryActors = 0;
	int32 LightCount = 0;

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor || Actor->IsPendingKillPending())
		{
			continue;
		}

		TotalActors++;

		FString ClassName = Actor->GetClass()->GetName();
		ActorCountsByClass.FindOrAdd(ClassName)++;

		// Count mobility
		if (USceneComponent* RootComp = Actor->GetRootComponent())
		{
			switch (RootComp->Mobility)
			{
			case EComponentMobility::Static:
				StaticActors++;
				break;
			case EComponentMobility::Stationary:
				StationaryActors++;
				break;
			case EComponentMobility::Movable:
				MovableActors++;
				break;
			}
		}

		// Count lights
		if (Cast<ALight>(Actor))
		{
			LightCount++;
		}

		// Count vertices/triangles for static meshes
		if (AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(Actor))
		{
			if (UStaticMeshComponent* MeshComp = StaticMeshActor->GetStaticMeshComponent())
			{
				if (UStaticMesh* Mesh = MeshComp->GetStaticMesh())
				{
					if (Mesh->GetRenderData())
					{
						for (const FStaticMeshLODResources& LOD : Mesh->GetRenderData()->LODResources)
						{
							TotalVertices += LOD.GetNumVertices();
							TotalTriangles += LOD.GetNumTriangles();
						}
					}
				}
			}
		}
	}

	// Sort by count (descending)
	ActorCountsByClass.ValueSort([](int32 A, int32 B) { return A > B; });

	// Build result message
	TArray<FString> TopClasses;
	int32 Count = 0;
	for (const auto& Pair : ActorCountsByClass)
	{
		TopClasses.Add(FString::Printf(TEXT("  - %s: %d"), *Pair.Key, Pair.Value));
		if (++Count >= 10) break; // Top 10
	}

	FString Message = FString::Printf(
		TEXT("Level Statistics:\n"
		"Total Actors: %d\n"
		"Static: %d, Stationary: %d, Movable: %d\n"
		"Lights: %d\n"
		"Total Vertices: %lld\n"
		"Total Triangles: %lld\n"
		"Top Actor Classes:\n%s"),
		TotalActors,
		StaticActors, StationaryActors, MovableActors,
		LightCount,
		TotalVertices,
		TotalTriangles,
		*FString::Join(TopClasses, TEXT("\n"))
	);

	UE_LOG(LogSmartUEAssistantTools, Log, TEXT("Level Analysis:\n%s"), *Message);

	// Build structured data
	TSharedPtr<FJsonObject> ResultData = MakeShareable(new FJsonObject);
	ResultData->SetNumberField(TEXT("TotalActors"), TotalActors);
	ResultData->SetNumberField(TEXT("StaticActors"), StaticActors);
	ResultData->SetNumberField(TEXT("StationaryActors"), StationaryActors);
	ResultData->SetNumberField(TEXT("MovableActors"), MovableActors);
	ResultData->SetNumberField(TEXT("LightCount"), LightCount);
	ResultData->SetNumberField(TEXT("TotalVertices"), static_cast<double>(TotalVertices));
	ResultData->SetNumberField(TEXT("TotalTriangles"), static_cast<double>(TotalTriangles));

	TSharedPtr<FJsonObject> ClassCountsObj = MakeShareable(new FJsonObject);
	for (const auto& Pair : ActorCountsByClass)
	{
		ClassCountsObj->SetNumberField(Pair.Key, Pair.Value);
	}
	ResultData->SetObjectField(TEXT("ActorCountsByClass"), ClassCountsObj);

	return {true, Message, ResultData};
}

// ==================== Find Missing References Tool ====================

FFindMissingReferencesTool::FFindMissingReferencesTool()
{
	Spec.Name = TEXT("find_missing_references");
	Spec.Description = TEXT("Find actors with missing asset references in the level");
	Spec.Permission = EToolPermission::Safe;
	Spec.bRequireConfirm = false;
}

FAIToolResult FFindMissingReferencesTool::Execute(const TSharedPtr<FJsonObject>& Args)
{
	if (!GEditor)
	{
		return {false, TEXT("Editor not available"), nullptr};
	}

	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World)
	{
		return {false, TEXT("No world available"), nullptr};
	}

	TArray<FString> ActorsWithMissingRefs;

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor || Actor->IsPendingKillPending())
		{
			continue;
		}

		bool bHasMissingRef = false;

		// Check static mesh
		if (AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(Actor))
		{
			if (UStaticMeshComponent* MeshComp = StaticMeshActor->GetStaticMeshComponent())
			{
				if (!MeshComp->GetStaticMesh())
				{
					bHasMissingRef = true;
				}
				else
				{
					// Check materials
					for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
					{
						if (!MeshComp->GetMaterial(i))
						{
							bHasMissingRef = true;
							break;
						}
					}
				}
			}
		}

		if (bHasMissingRef)
		{
			ActorsWithMissingRefs.Add(FString::Printf(TEXT("  - %s (%s)"), 
				*Actor->GetActorLabel(), *Actor->GetClass()->GetName()));
		}
	}

	FString Message;
	if (ActorsWithMissingRefs.Num() > 0)
	{
		Message = FString::Printf(TEXT("Found %d actors with missing references:\n%s"),
			ActorsWithMissingRefs.Num(),
			*FString::Join(ActorsWithMissingRefs, TEXT("\n")));
	}
	else
	{
		Message = TEXT("No actors with missing references found");
	}

	UE_LOG(LogSmartUEAssistantTools, Log, TEXT("%s"), *Message);

	TSharedPtr<FJsonObject> ResultData = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> ActorArray;
	for (const FString& ActorName : ActorsWithMissingRefs)
	{
		ActorArray.Add(MakeShareable(new FJsonValueString(ActorName)));
	}
	ResultData->SetArrayField(TEXT("ActorsWithMissingRefs"), ActorArray);

	return {true, Message, ResultData};
}

// ==================== Find Duplicate Names Tool ====================

FFindDuplicateNamesTool::FFindDuplicateNamesTool()
{
	Spec.Name = TEXT("find_duplicate_names");
	Spec.Description = TEXT("Find actors with duplicate names in the level");
	Spec.Permission = EToolPermission::Safe;
	Spec.bRequireConfirm = false;
}

FAIToolResult FFindDuplicateNamesTool::Execute(const TSharedPtr<FJsonObject>& Args)
{
	if (!GEditor)
	{
		return {false, TEXT("Editor not available"), nullptr};
	}

	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World)
	{
		return {false, TEXT("No world available"), nullptr};
	}

	TMap<FString, TArray<AActor*>> NameToActors;

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor || Actor->IsPendingKillPending())
		{
			continue;
		}

		FString Name = Actor->GetActorLabel();
		NameToActors.FindOrAdd(Name).Add(Actor);
	}

	TArray<FString> DuplicateNames;
	for (const auto& Pair : NameToActors)
	{
		if (Pair.Value.Num() > 1)
		{
			DuplicateNames.Add(FString::Printf(TEXT("  - '%s': %d instances"), 
				*Pair.Key, Pair.Value.Num()));
		}
	}

	FString Message;
	if (DuplicateNames.Num() > 0)
	{
		Message = FString::Printf(TEXT("Found %d duplicate actor names:\n%s"),
			DuplicateNames.Num(),
			*FString::Join(DuplicateNames, TEXT("\n")));
	}
	else
	{
		Message = TEXT("No duplicate actor names found");
	}

	UE_LOG(LogSmartUEAssistantTools, Log, TEXT("%s"), *Message);

	TSharedPtr<FJsonObject> ResultData = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> DuplicatesArray;
	for (const FString& DupName : DuplicateNames)
	{
		DuplicatesArray.Add(MakeShareable(new FJsonValueString(DupName)));
	}
	ResultData->SetArrayField(TEXT("DuplicateNames"), DuplicatesArray);

	return {true, Message, ResultData};
}

// ==================== Find Oversized Meshes Tool ====================

FFindOversizedMeshesTool::FFindOversizedMeshesTool()
{
	Spec.Name = TEXT("find_oversized_meshes");
	Spec.Description = TEXT("Find actors with high-poly meshes that may impact performance");
	Spec.Permission = EToolPermission::Safe;
	Spec.bRequireConfirm = false;
	
	Spec.Params.Add({TEXT("VertexThreshold"), TEXT("number"), true, TEXT("Minimum vertex count to flag (default: 50000)")});
}

FAIToolResult FFindOversizedMeshesTool::Execute(const TSharedPtr<FJsonObject>& Args)
{
	if (!GEditor)
	{
		return {false, TEXT("Editor not available"), nullptr};
	}

	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World)
	{
		return {false, TEXT("No world available"), nullptr};
	}

	int32 VertexThreshold = Args->HasField(TEXT("VertexThreshold")) ? 
		static_cast<int32>(Args->GetNumberField(TEXT("VertexThreshold"))) : 50000;

	TArray<FString> OversizedMeshes;

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor || Actor->IsPendingKillPending())
		{
			continue;
		}

		if (AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(Actor))
		{
			if (UStaticMeshComponent* MeshComp = StaticMeshActor->GetStaticMeshComponent())
			{
				if (UStaticMesh* Mesh = MeshComp->GetStaticMesh())
				{
					if (Mesh->GetRenderData())
					{
						int64 VertexCount = 0;
						for (const FStaticMeshLODResources& LOD : Mesh->GetRenderData()->LODResources)
						{
							VertexCount += LOD.GetNumVertices();
						}

						if (VertexCount > VertexThreshold)
						{
							OversizedMeshes.Add(FString::Printf(TEXT("  - %s: %lld vertices (%s)"),
								*Actor->GetActorLabel(), VertexCount, *Mesh->GetName()));
						}
					}
				}
			}
		}
	}

	FString Message;
	if (OversizedMeshes.Num() > 0)
	{
		Message = FString::Printf(TEXT("Found %d oversized meshes (threshold: %d vertices):\n%s"),
			OversizedMeshes.Num(), VertexThreshold,
			*FString::Join(OversizedMeshes, TEXT("\n")));
	}
	else
	{
		Message = FString::Printf(TEXT("No oversized meshes found (threshold: %d vertices)"), VertexThreshold);
	}

	UE_LOG(LogSmartUEAssistantTools, Log, TEXT("%s"), *Message);

	TSharedPtr<FJsonObject> ResultData = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> MeshesArray;
	for (const FString& MeshInfo : OversizedMeshes)
	{
		MeshesArray.Add(MakeShareable(new FJsonValueString(MeshInfo)));
	}
	ResultData->SetArrayField(TEXT("OversizedMeshes"), MeshesArray);
	ResultData->SetNumberField(TEXT("Threshold"), VertexThreshold);

	return {true, Message, ResultData};
}

// ==================== Validate Level Tool ====================

FValidateLevelTool::FValidateLevelTool()
{
	Spec.Name = TEXT("validate_level");
	Spec.Description = TEXT("Validate level for common issues including collision, lighting, and mobility settings");
	Spec.Permission = EToolPermission::Safe;
	Spec.bRequireConfirm = false;
}

FAIToolResult FValidateLevelTool::Execute(const TSharedPtr<FJsonObject>& Args)
{
	if (!GEditor)
	{
		return {false, TEXT("Editor not available"), nullptr};
	}

	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World)
	{
		return {false, TEXT("No world available"), nullptr};
	}

	TArray<FString> Issues;
	int32 TotalChecked = 0;

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor || Actor->IsPendingKillPending())
		{
			continue;
		}

		TotalChecked++;

		// Check for missing collision on static meshes
		if (AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(Actor))
		{
			if (UStaticMeshComponent* MeshComp = StaticMeshActor->GetStaticMeshComponent())
			{
				if (MeshComp->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
				{
					Issues.Add(FString::Printf(TEXT("  [No Collision] %s"), *Actor->GetActorLabel()));
				}

				// Check for improper mobility
				if (UStaticMesh* Mesh = MeshComp->GetStaticMesh())
				{
					if (MeshComp->Mobility == EComponentMobility::Movable && 
						Mesh->GetRenderData() && 
						Mesh->GetRenderData()->LODResources[0].GetNumVertices() > 10000)
					{
						Issues.Add(FString::Printf(TEXT("  [High-Poly Movable] %s - Consider making static"), *Actor->GetActorLabel()));
					}
				}
			}
		}

		// Check for lights without shadows
		if (ALight* LightActor = Cast<ALight>(Actor))
		{
			if (ULightComponent* LightComp = LightActor->GetLightComponent())
			{
				if (!LightComp->CastShadows)
				{
					Issues.Add(FString::Printf(TEXT("  [No Shadows] %s light"), *Actor->GetActorLabel()));
				}
			}
		}

		// Check for actors outside reasonable bounds
		FVector Location = Actor->GetActorLocation();
		const float MaxBounds = 1000000.0f; // 10km
		if (FMath::Abs(Location.X) > MaxBounds || 
			FMath::Abs(Location.Y) > MaxBounds || 
			FMath::Abs(Location.Z) > MaxBounds)
		{
			Issues.Add(FString::Printf(TEXT("  [Outside Bounds] %s at (%.1f, %.1f, %.1f)"),
				*Actor->GetActorLabel(), Location.X, Location.Y, Location.Z));
		}
	}

	FString Message;
	if (Issues.Num() > 0)
	{
		Message = FString::Printf(TEXT("Level Validation - Found %d issues (Checked %d actors):\n%s"),
			Issues.Num(), TotalChecked,
			*FString::Join(Issues, TEXT("\n")));
	}
	else
	{
		Message = FString::Printf(TEXT("Level Validation - No issues found (Checked %d actors)"), TotalChecked);
	}

	UE_LOG(LogSmartUEAssistantTools, Log, TEXT("%s"), *Message);

	TSharedPtr<FJsonObject> ResultData = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> IssuesArray;
	for (const FString& Issue : Issues)
	{
		IssuesArray.Add(MakeShareable(new FJsonValueString(Issue)));
	}
	ResultData->SetArrayField(TEXT("Issues"), IssuesArray);
	ResultData->SetNumberField(TEXT("TotalChecked"), TotalChecked);
	ResultData->SetNumberField(TEXT("IssueCount"), Issues.Num());

	return {true, Message, ResultData};
}


// �?自动注册工具
#include "ToolAutoRegister.h"

REGISTER_EDITOR_TOOL(FAnalyzeLevelStatsTool)
REGISTER_EDITOR_TOOL(FFindMissingReferencesTool)
REGISTER_EDITOR_TOOL(FFindDuplicateNamesTool)
REGISTER_EDITOR_TOOL(FFindOversizedMeshesTool)
REGISTER_EDITOR_TOOL(FValidateLevelTool)

