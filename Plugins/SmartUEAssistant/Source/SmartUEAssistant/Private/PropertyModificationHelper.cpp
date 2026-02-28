// Copyright Epic Games, Inc. All Rights Reserved.

#include "PropertyModificationHelper.h"
#include "SmartUEAssistantLog.h"
#include "Editor.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "Components/LightComponent.h"
#include "Components/SceneComponent.h"
#include "UObject/UnrealType.h"
#include "Dom/JsonObject.h"

bool FPropertyModificationHelper::ParseColor(const TSharedPtr<FJsonValue>& Value, FLinearColor& OutColor)
{
	if (!Value.IsValid())
	{
		return false;
	}

	if (Value->Type == EJson::Object)
	{
		const TSharedPtr<FJsonObject>& Obj = Value->AsObject();
		if (Obj->HasField(TEXT("R")) && Obj->HasField(TEXT("G")) && Obj->HasField(TEXT("B")))
		{
			OutColor.R = Obj->GetNumberField(TEXT("R"));
			OutColor.G = Obj->GetNumberField(TEXT("G"));
			OutColor.B = Obj->GetNumberField(TEXT("B"));
			OutColor.A = Obj->HasField(TEXT("A")) ? Obj->GetNumberField(TEXT("A")) : 1.0f;
			return true;
		}
	}
	else if (Value->Type == EJson::String)
	{
		FString ColorName = Value->AsString().ToLower();
		
		// 支持中英文颜色名
		if (ColorName == TEXT("red") || ColorName == TEXT("红色"))
			OutColor = FLinearColor::Red;
		else if (ColorName == TEXT("green") || ColorName == TEXT("绿色"))
			OutColor = FLinearColor::Green;
		else if (ColorName == TEXT("blue") || ColorName == TEXT("蓝色"))
			OutColor = FLinearColor::Blue;
		else if (ColorName == TEXT("white") || ColorName == TEXT("白色"))
			OutColor = FLinearColor::White;
		else if (ColorName == TEXT("black") || ColorName == TEXT("黑色"))
			OutColor = FLinearColor::Black;
		else if (ColorName == TEXT("yellow") || ColorName == TEXT("黄色"))
			OutColor = FLinearColor::Yellow;
		else if (ColorName == TEXT("cyan") || ColorName == TEXT("青色"))
			OutColor = FLinearColor(0, 1, 1);
		else if (ColorName == TEXT("magenta") || ColorName == TEXT("洋红"))
			OutColor = FLinearColor(1, 0, 1);
		else if (ColorName == TEXT("orange") || ColorName == TEXT("橙色"))
			OutColor = FLinearColor(1, 0.5f, 0);
		else if (ColorName == TEXT("purple") || ColorName == TEXT("紫色"))
			OutColor = FLinearColor(0.5f, 0, 1);
		else if (ColorName == TEXT("pink") || ColorName == TEXT("粉色"))
			OutColor = FLinearColor(1, 0.5f, 0.5f);
		else if (ColorName == TEXT("brown") || ColorName == TEXT("棕色"))
			OutColor = FLinearColor(0.6f, 0.3f, 0.0f);
		else if (ColorName == TEXT("gray") || ColorName == TEXT("grey") || ColorName == TEXT("灰色"))
			OutColor = FLinearColor(0.5f, 0.5f, 0.5f);
		else
			return false;
		
		return true;
	}
	
	return false;
}

bool FPropertyModificationHelper::ParseVector(const TSharedPtr<FJsonValue>& Value, FVector& OutVector)
{
	if (!Value.IsValid() || Value->Type != EJson::Object)
	{
		return false;
	}

	const TSharedPtr<FJsonObject>& Obj = Value->AsObject();
	if (Obj->HasField(TEXT("X")) && Obj->HasField(TEXT("Y")) && Obj->HasField(TEXT("Z")))
	{
		OutVector.X = Obj->GetNumberField(TEXT("X"));
		OutVector.Y = Obj->GetNumberField(TEXT("Y"));
		OutVector.Z = Obj->GetNumberField(TEXT("Z"));
		return true;
	}
	
	return false;
}

bool FPropertyModificationHelper::ParseRotator(const TSharedPtr<FJsonValue>& Value, FRotator& OutRotator)
{
	if (!Value.IsValid() || Value->Type != EJson::Object)
	{
		return false;
	}

	const TSharedPtr<FJsonObject>& Obj = Value->AsObject();
	if (Obj->HasField(TEXT("Pitch")) && Obj->HasField(TEXT("Yaw")) && Obj->HasField(TEXT("Roll")))
	{
		OutRotator.Pitch = Obj->GetNumberField(TEXT("Pitch"));
		OutRotator.Yaw = Obj->GetNumberField(TEXT("Yaw"));
		OutRotator.Roll = Obj->GetNumberField(TEXT("Roll"));
		return true;
	}
	
	return false;
}

FProperty* FPropertyModificationHelper::FindPropertyByPath(UObject* Object, const FString& PropertyPath, UObject*& OutContainer)
{
	UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  >> FindPropertyByPath: %s"), *PropertyPath);

	if (!Object || PropertyPath.IsEmpty())
	{
		UE_LOG(LogSmartUEAssistantTools, Error, TEXT("  >> Invalid input: Object=%s, Path=%s"), 
			Object ? TEXT("Valid") : TEXT("NULL"), 
			PropertyPath.IsEmpty() ? TEXT("Empty") : *PropertyPath);
		return nullptr;
	}

	TArray<FString> PathParts;
	PropertyPath.ParseIntoArray(PathParts, TEXT("."));
	UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  >> Path parts: %d"), PathParts.Num());

	OutContainer = Object;
	FProperty* Property = nullptr;

	for (int32 i = 0; i < PathParts.Num(); ++i)
	{
		const FString& PartName = PathParts[i];
		UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  >> Part[%d]: %s (Container: %s)"), 
			i, *PartName, *OutContainer->GetClass()->GetName());
		
		// Try to find property in current container's class
		Property = FindFProperty<FProperty>(OutContainer->GetClass(), *PartName);
		
		if (Property)
		{
			UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  >> Found property: %s (Type: %s)"), 
				*Property->GetNameCPP(), *Property->GetClass()->GetName());
		}
		else
		{
			UE_LOG(LogSmartUEAssistantTools, Warning, TEXT("  >> Property not found: %s"), *PartName);
		}
		
		// If not found and not the last part, try to find as component
		if (!Property && i < PathParts.Num() - 1)
		{
			if (AActor* Actor = Cast<AActor>(OutContainer))
			{
				UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  >> Searching components for: %s"), *PartName);
				
				// Try to find component by name or class name
				TArray<UActorComponent*> Components;
				Actor->GetComponents(Components);
				
				UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  >> Actor has %d components"), Components.Num());
				
				for (UActorComponent* Component : Components)
				{
					if (Component)
					{
						FString CompName = Component->GetName();
						FString CompClassName = Component->GetClass()->GetName();
						
						if (CompName.Equals(PartName, ESearchCase::IgnoreCase) ||
						    CompClassName.Contains(PartName))
						{
							OutContainer = Component;
							UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  >> Found component: %s (Class: %s)"), 
								*CompName, *CompClassName);
							break;
						}
					}
				}
			}
			continue;
		}
		
		// If not found, return nullptr
		if (!Property)
		{
			UE_LOG(LogSmartUEAssistantTools, Error, TEXT("  >> Failed to find property: %s"), *PartName);
			return nullptr;
		}
		
		// If not the last part, navigate deeper
		if (i < PathParts.Num() - 1)
		{
			if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Property))
			{
				UObject* NextContainer = ObjProp->GetObjectPropertyValue(Property->ContainerPtrToValuePtr<void>(OutContainer));
				if (!NextContainer)
				{
					UE_LOG(LogSmartUEAssistantTools, Error, TEXT("  >> Next container is null"));
					return nullptr;
				}
				OutContainer = NextContainer;
				UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  >> Navigated to: %s"), *NextContainer->GetName());
			}
			else if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
			{
				UE_LOG(LogSmartUEAssistantTools, Error, TEXT("  >> Cannot navigate into struct property"));
				return nullptr;
			}
			else
			{
				UE_LOG(LogSmartUEAssistantTools, Error, TEXT("  >> Cannot navigate property type: %s"), 
					*Property->GetClass()->GetName());
				return nullptr;
			}
		}
	}

	UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  >> FindPropertyByPath SUCCESS: %s"), 
		Property ? *Property->GetNameCPP() : TEXT("NULL"));
	return Property;
}

bool FPropertyModificationHelper::SetPropertyValueDirect(
	FProperty* Property,
	void* ContainerPtr,
	const TSharedPtr<FJsonValue>& Value,
	FString& OutError)
{
	if (!Property || !ContainerPtr || !Value.IsValid())
	{
		OutError = TEXT("Invalid parameters");
		return false;
	}

	void* ValuePtr = Property->ContainerPtrToValuePtr<void>(ContainerPtr);

	// Handle different property types
	if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
	{
		if (Value->Type == EJson::Number)
		{
			FloatProp->SetPropertyValue(ValuePtr, Value->AsNumber());
			return true;
		}
	}
	else if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Property))
	{
		if (Value->Type == EJson::Number)
		{
			DoubleProp->SetPropertyValue(ValuePtr, Value->AsNumber());
			return true;
		}
	}
	else if (FIntProperty* IntProp = CastField<FIntProperty>(Property))
	{
		if (Value->Type == EJson::Number)
		{
			IntProp->SetPropertyValue(ValuePtr, static_cast<int32>(Value->AsNumber()));
			return true;
		}
	}
	else if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
	{
		if (Value->Type == EJson::Boolean)
		{
			BoolProp->SetPropertyValue(ValuePtr, Value->AsBool());
			return true;
		}
	}
	else if (FStrProperty* StrProp = CastField<FStrProperty>(Property))
	{
		if (Value->Type == EJson::String)
		{
			StrProp->SetPropertyValue(ValuePtr, Value->AsString());
			return true;
		}
	}
	else if (FNameProperty* NameProp = CastField<FNameProperty>(Property))
	{
		if (Value->Type == EJson::String)
		{
			NameProp->SetPropertyValue(ValuePtr, FName(*Value->AsString()));
			return true;
		}
	}
	else if (FTextProperty* TextProp = CastField<FTextProperty>(Property))
	{
		if (Value->Type == EJson::String)
		{
			TextProp->SetPropertyValue(ValuePtr, FText::FromString(Value->AsString()));
			return true;
		}
	}
	else if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
	{
		FString StructName = StructProp->Struct->GetName();
		
		// FVector
		if (StructName == TEXT("Vector"))
		{
			FVector Vector;
			if (ParseVector(Value, Vector))
			{
				*static_cast<FVector*>(ValuePtr) = Vector;
				return true;
			}
		}
		// FRotator
		else if (StructName == TEXT("Rotator"))
		{
			FRotator Rotator;
			if (ParseRotator(Value, Rotator))
			{
				*static_cast<FRotator*>(ValuePtr) = Rotator;
				return true;
			}
		}
		// FLinearColor
		else if (StructName == TEXT("LinearColor"))
		{
			FLinearColor Color;
			if (ParseColor(Value, Color))
			{
				*static_cast<FLinearColor*>(ValuePtr) = Color;
				return true;
			}
		}
		// FColor
		else if (StructName == TEXT("Color"))
		{
			FLinearColor LinearColor;
			if (ParseColor(Value, LinearColor))
			{
				*static_cast<FColor*>(ValuePtr) = LinearColor.ToFColor(true);
				return true;
			}
		}
	}
	else if (FEnumProperty* EnumProp = CastField<FEnumProperty>(Property))
	{
		if (Value->Type == EJson::String)
		{
			UEnum* Enum = EnumProp->GetEnum();
			int64 EnumValue = Enum->GetValueByNameString(Value->AsString());
			if (EnumValue != INDEX_NONE)
			{
				EnumProp->GetUnderlyingProperty()->SetIntPropertyValue(ValuePtr, EnumValue);
				return true;
			}
		}
		else if (Value->Type == EJson::Number)
		{
			EnumProp->GetUnderlyingProperty()->SetIntPropertyValue(ValuePtr, static_cast<int64>(Value->AsNumber()));
			return true;
		}
	}
	else if (FByteProperty* ByteProp = CastField<FByteProperty>(Property))
	{
		if (ByteProp->Enum)
		{
			// Enum byte property
			if (Value->Type == EJson::String)
			{
				int64 EnumValue = ByteProp->Enum->GetValueByNameString(Value->AsString());
				if (EnumValue != INDEX_NONE)
				{
					ByteProp->SetPropertyValue(ValuePtr, static_cast<uint8>(EnumValue));
					return true;
				}
			}
		}
		
		if (Value->Type == EJson::Number)
		{
			ByteProp->SetPropertyValue(ValuePtr, static_cast<uint8>(Value->AsNumber()));
			return true;
		}
	}

	OutError = FString::Printf(TEXT("Unsupported property type: %s"), *Property->GetClass()->GetName());
	return false;
}

bool FPropertyModificationHelper::SetPropertyValue(
	UObject* Object,
	const FString& PropertyPath,
	const TSharedPtr<FJsonValue>& Value,
	bool bCreateTransaction,
	const FString& TransactionDescription,
	FString& OutError)
{
	UE_LOG(LogSmartUEAssistantTools, Log, TEXT("=== SetPropertyValue START ==="));
	UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  Object: %s (Class: %s)"), 
		Object ? *Object->GetName() : TEXT("NULL"), 
		Object ? *Object->GetClass()->GetName() : TEXT("NULL"));
	UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  PropertyPath: %s"), *PropertyPath);
	UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  CreateTransaction: %s"), bCreateTransaction ? TEXT("TRUE") : TEXT("FALSE"));

	if (!Object)
	{
		OutError = TEXT("Invalid object");
		UE_LOG(LogSmartUEAssistantTools, Error, TEXT("  ERROR: Invalid object"));
		return false;
	}

	// Find the property
	UObject* Container = nullptr;
	FProperty* Property = FindPropertyByPath(Object, PropertyPath, Container);

	if (!Property || !Container)
	{
		OutError = FString::Printf(TEXT("Property not found: %s on %s"), *PropertyPath, *Object->GetName());
		UE_LOG(LogSmartUEAssistantTools, Error, TEXT("  ERROR: %s"), *OutError);
		UE_LOG(LogSmartUEAssistantTools, Error, TEXT("  Property: %s, Container: %s"), 
			Property ? *Property->GetNameCPP() : TEXT("NULL"),
			Container ? *Container->GetName() : TEXT("NULL"));
		return false;
	}

	UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  Found Property: %s (Type: %s)"), 
		*Property->GetNameCPP(), *GetPropertyTypeName(Property));
	UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  Container: %s (Class: %s)"), 
		*Container->GetName(), *Container->GetClass()->GetName());

	// Check if property is editable
	if (!IsPropertyEditable(Property))
	{
		OutError = FString::Printf(TEXT("Property is not editable: %s"), *PropertyPath);
		UE_LOG(LogSmartUEAssistantTools, Error, TEXT("  ERROR: %s"), *OutError);
		return false;
	}

	// Create transaction for undo/redo
	FScopedTransaction* Transaction = nullptr;
	if (bCreateTransaction)
	{
		Transaction = new FScopedTransaction(FText::FromString(TransactionDescription));
		UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  Created transaction: %s"), *TransactionDescription);
	}

	// Mark object as modified (for undo/redo and dirty flag)
	Container->Modify();
	UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  Called Modify() on container"));

	// Set the property value
	bool bSuccess = SetPropertyValueDirect(Property, Container, Value, OutError);

	if (bSuccess)
	{
		UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  ✓ Property value set successfully"));

		// **THIS IS THE KEY**: Call PostEditChangeProperty to trigger all updates
		UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  Calling PostEditChangeProperty..."));
		FPropertyChangedEvent PropertyChangedEvent(Property, EPropertyChangeType::ValueSet);
		Container->PostEditChangeProperty(PropertyChangedEvent);
		UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  ✓ PostEditChangeProperty called"));

		// For components, also mark render state dirty
		if (UActorComponent* Component = Cast<UActorComponent>(Container))
		{
			Component->MarkRenderStateDirty();
			UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  ✓ MarkRenderStateDirty called (Component)"));
		}

		// For scene components, update component to world
		if (USceneComponent* SceneComp = Cast<USceneComponent>(Container))
		{
			SceneComp->UpdateComponentToWorld();
			UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  ✓ UpdateComponentToWorld called (SceneComponent)"));
		}

		// For light components, log additional info
		if (ULightComponent* LightComp = Cast<ULightComponent>(Container))
		{
			UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  Light Color: R=%f G=%f B=%f"), 
				(float)LightComp->LightColor.R, (float)LightComp->LightColor.G, (float)LightComp->LightColor.B);
			UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  Light Intensity: %f"), (float)LightComp->Intensity);
		}

		UE_LOG(LogSmartUEAssistantTools, Log, TEXT("=== SetPropertyValue SUCCESS ==="));
	}
	else
	{
		UE_LOG(LogSmartUEAssistantTools, Error, TEXT("  ✗ SetPropertyValueDirect failed: %s"), *OutError);
		
		// Cancel transaction on failure
		if (Transaction)
		{
			Transaction->Cancel();
			UE_LOG(LogSmartUEAssistantTools, Log, TEXT("  Transaction cancelled"));
		}
		
		delete Transaction;
		UE_LOG(LogSmartUEAssistantTools, Error, TEXT("=== SetPropertyValue FAILED ==="));
		return false;
	}

	delete Transaction;
	return true;
}

TSharedPtr<FJsonValue> FPropertyModificationHelper::GetPropertyValueAsJson(FProperty* Property, const void* ContainerPtr)
{
	if (!Property || !ContainerPtr)
	{
		return nullptr;
	}

	const void* ValuePtr = Property->ContainerPtrToValuePtr<const void>(ContainerPtr);

	if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
	{
		return MakeShared<FJsonValueNumber>(FloatProp->GetPropertyValue(ValuePtr));
	}
	else if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Property))
	{
		return MakeShared<FJsonValueNumber>(DoubleProp->GetPropertyValue(ValuePtr));
	}
	else if (FIntProperty* IntProp = CastField<FIntProperty>(Property))
	{
		return MakeShared<FJsonValueNumber>(IntProp->GetPropertyValue(ValuePtr));
	}
	else if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
	{
		return MakeShared<FJsonValueBoolean>(BoolProp->GetPropertyValue(ValuePtr));
	}
	else if (FStrProperty* StrProp = CastField<FStrProperty>(Property))
	{
		return MakeShared<FJsonValueString>(StrProp->GetPropertyValue(ValuePtr));
	}
	else if (FNameProperty* NameProp = CastField<FNameProperty>(Property))
	{
		return MakeShared<FJsonValueString>(NameProp->GetPropertyValue(ValuePtr).ToString());
	}
	else if (FTextProperty* TextProp = CastField<FTextProperty>(Property))
	{
		return MakeShared<FJsonValueString>(TextProp->GetPropertyValue(ValuePtr).ToString());
	}
	else if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
	{
		FString StructName = StructProp->Struct->GetName();
		
		if (StructName == TEXT("Vector"))
		{
			const FVector& Vector = *static_cast<const FVector*>(ValuePtr);
			TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
			Obj->SetNumberField(TEXT("X"), Vector.X);
			Obj->SetNumberField(TEXT("Y"), Vector.Y);
			Obj->SetNumberField(TEXT("Z"), Vector.Z);
			return MakeShared<FJsonValueObject>(Obj);
		}
		else if (StructName == TEXT("Rotator"))
		{
			const FRotator& Rotator = *static_cast<const FRotator*>(ValuePtr);
			TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
			Obj->SetNumberField(TEXT("Pitch"), Rotator.Pitch);
			Obj->SetNumberField(TEXT("Yaw"), Rotator.Yaw);
			Obj->SetNumberField(TEXT("Roll"), Rotator.Roll);
			return MakeShared<FJsonValueObject>(Obj);
		}
		else if (StructName == TEXT("LinearColor"))
		{
			const FLinearColor& Color = *static_cast<const FLinearColor*>(ValuePtr);
			TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
			Obj->SetNumberField(TEXT("R"), Color.R);
			Obj->SetNumberField(TEXT("G"), Color.G);
			Obj->SetNumberField(TEXT("B"), Color.B);
			Obj->SetNumberField(TEXT("A"), Color.A);
			return MakeShared<FJsonValueObject>(Obj);
		}
	}

	return MakeShared<FJsonValueString>(TEXT("<unsupported type>"));
}

bool FPropertyModificationHelper::IsPropertyEditable(FProperty* Property)
{
	if (!Property)
	{
		return false;
	}

	// Check property flags
	if (Property->HasAnyPropertyFlags(CPF_BlueprintReadOnly | CPF_EditConst))
	{
		return false;
	}

	return true;
}

FString FPropertyModificationHelper::GetPropertyTypeName(FProperty* Property)
{
	if (!Property)
	{
		return TEXT("Unknown");
	}

	if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
	{
		return TEXT("Float");
	}
	else if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Property))
	{
		return TEXT("Double");
	}
	else if (FIntProperty* IntProp = CastField<FIntProperty>(Property))
	{
		return TEXT("Int");
	}
	else if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
	{
		return TEXT("Bool");
	}
	else if (FStrProperty* StrProp = CastField<FStrProperty>(Property))
	{
		return TEXT("String");
	}
	else if (FNameProperty* NameProp = CastField<FNameProperty>(Property))
	{
		return TEXT("Name");
	}
	else if (FTextProperty* TextProp = CastField<FTextProperty>(Property))
	{
		return TEXT("Text");
	}
	else if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
	{
		return FString::Printf(TEXT("Struct<%s>"), *StructProp->Struct->GetName());
	}
	else if (FEnumProperty* EnumProp = CastField<FEnumProperty>(Property))
	{
		return FString::Printf(TEXT("Enum<%s>"), *EnumProp->GetEnum()->GetName());
	}
	else if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Property))
	{
		return FString::Printf(TEXT("Object<%s>"), *ObjProp->PropertyClass->GetName());
	}

	return Property->GetClass()->GetName();
}

