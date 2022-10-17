// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EPlayerType : uint8
{
	CrowPlayer,
	PhoenixPlayer
};

UENUM(BlueprintType)
enum class EIslandPosition : uint8
{
	IslandA,  
	IslandB,
	IslandC,
	IslandD,
	Count UMETA(Hidden) 
};

