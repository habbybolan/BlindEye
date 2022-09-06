// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MarkData.generated.h"

enum class PlayerType : uint8;

/**
 * Class for storing Mark data needed for detonating the effect
 */
USTRUCT()
struct BLINDEYE_API FMarkData
{
	GENERATED_BODY()

	FMarkData();

	void InitializeData(PlayerType markPlayerType);
	
	PlayerType MarkPlayerType;
	uint8 UniqueAbilityMarkerIndex;

	
	
};
