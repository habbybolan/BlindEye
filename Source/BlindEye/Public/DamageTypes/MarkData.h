// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/MarkerComponent.h"
#include "UObject/NoExportTypes.h"
#include "MarkData.generated.h"

enum class EPlayerType : uint8;

/**
 * Class for storing Mark data needed for detonating the effect
 */
USTRUCT()
struct BLINDEYE_API FMarkData
{
	GENERATED_BODY();
 
	void InitializeData(EMarkerType MarkerType);
	
	EMarkerType MarkerType = EMarkerType::Crow;
	uint8 UniqueAbilityMarkerIndex;

	
	
};
