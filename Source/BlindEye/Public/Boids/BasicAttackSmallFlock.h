// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Boids/Flock.h"
#include "BasicAttackSmallFlock.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API ABasicAttackSmallFlock : public AFlock
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TargetDistanceFromInstigator = 2000.f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> TargetType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ETraceTypeQuery> MouseRayTrace;

protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	void CheckForDamage();
	void CheckRemoveTarget();
	
};
