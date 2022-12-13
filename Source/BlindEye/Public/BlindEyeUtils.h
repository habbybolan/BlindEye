// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BlindEyeUtils.generated.h"

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
	IslandShrine,
	Count UMETA(Hidden) 
};

USTRUCT()
struct FTutorialActionBlockers 
{
	GENERATED_BODY()

	UPROPERTY()
	bool bUnique1Blocked = false;
	UPROPERTY()
	bool bUnique2blocked = false;
	UPROPERTY()
	bool bDashBlocked = false;
	UPROPERTY()
	bool bBasicAttackBlocked = false;
	UPROPERTY()
	bool bLocomotionBlocked = false;

	void Reset()
	{
		bUnique1Blocked = false;
		bUnique2blocked = false;
		bDashBlocked = false;
		bBasicAttackBlocked = false;
		bLocomotionBlocked = false;
	}
};

UENUM()
namespace TutorialInputActions
{
	enum ETutorialInputActions
	{
		Walk, 
		Jump,
		BasicAttack, 
		BasicAttackCombo,
		Dash,
	};
}

UENUM(BlueprintType) 
enum class EBurrowerVisibilityState : uint8
{
	Hidden,
	Hiding,
	Surfacing,
	Surfaced
};



