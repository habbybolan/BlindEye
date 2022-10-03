// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GroundTarget.generated.h"

UCLASS()
class BLINDEYE_API AGroundTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	AGroundTarget();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float TimeToMax = 1.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MaxRadius = 0.5f;

};
