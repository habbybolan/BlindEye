// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "NavLinkProxy_Jump.generated.h"

class ABlindEyeBaseCharacter;

/**
 * 
 */
UCLASS()
class BLINDEYE_API ANavLinkProxy_Jump : public ANavLinkProxy
{
	GENERATED_BODY()

	ANavLinkProxy_Jump(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	FVector CalculateDownwardVectorImpulse(ABlindEyeBaseCharacter* Enemy, FVector TargetPosition, float Angle);

	UFUNCTION()
	void OnSmartLinkReachedHelper(AActor* MovingActor, const FVector& DestinationPoint);
	
};
