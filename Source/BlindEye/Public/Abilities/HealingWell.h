// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealingWell.generated.h"

UCLASS()
class BLINDEYE_API AHealingWell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHealingWell();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HealPercentIncr = 1000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HealCheckDelay = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Radius = 450;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Duration = 4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditDefaultsOnly)
	float DelayedDestructionTime = 2;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_HealingWellDestroying();
	
protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	void PerformHealCheck();
	FTimerHandle HealingCheckTimerHandle; 

	FTimerHandle DelayedDestroyTimerHandle;

	FRotator CachedSpawnedRotation;

	virtual void BeginDestroy() override;

	void DelayedDestroy();

	void DestroyHealingWell();

};
