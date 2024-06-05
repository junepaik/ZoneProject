// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogZoneProject, Log, All);

/**
 * Area Event
 */
UENUM(BlueprintType)
enum class EDropItemType : uint8
{
	None                UMETA(DisplayName = "None"),
	Health              UMETA(DisplayName = "Health"),
	Max                 UMETA(Hidden)
};
