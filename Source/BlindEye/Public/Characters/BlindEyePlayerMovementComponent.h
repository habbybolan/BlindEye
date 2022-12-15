// Copyright (C) Nicholas Johnson 2022
#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "BlindEyePlayerMovementComponent.generated.h"

UCLASS()
class BLINDEYE_API UBlindEyePlayerMovementComponent : public UCharacterMovementComponent
{

	GENERATED_BODY()
	
public:

	UPROPERTY(Category="Character Movement (Rotation Settings)", EditAnywhere, BlueprintReadWrite)
	uint8 bOrientRotationToMouse:1;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void PhysicsRotation(float DeltaTime) override;
};
