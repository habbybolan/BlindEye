// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BlindEyeEnemyController.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API ABlindEyeEnemyController : public AAIController
{
	GENERATED_BODY()

public:



protected:
	virtual void BeginPlay() override;

	// Status effect delegate listeners ******
	
	UFUNCTION()
	virtual void OnStunStart(float StunDuration);
	UFUNCTION()
	virtual void OnStunEnd();

	UFUNCTION()
	virtual void OnBurnStart(float DamagePerSec, float Duration);
	UFUNCTION() 
	virtual void OnBurnEnd();

	UFUNCTION()
	virtual void OnTauntStart(float Duration, AActor* Taunter);
	UFUNCTION()  
	virtual void OnTauntEnd(); 

	// End Status effect delegate listeners ******

	virtual void OnPossess(APawn* InPawn) override;
	
};
