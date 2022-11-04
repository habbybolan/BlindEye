// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DifficultyManager.generated.h"

UCLASS()
class BLINDEYE_API ADifficultyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADifficultyManager();

	UPROPERTY(EditDefaultsOnly)
	TArray<float> BurrowerBaseSpawnRatePerRound;
 
	UPROPERTY(EditDefaultsOnly) 
	TArray<UCurveFloat*> BurrowerSpawnMultiplierPerRoundCurve;

	UPROPERTY(EditDefaultsOnly)
	float HunterBaseSpawnRate;

	UPROPERTY(EditDefaultsOnly) 
	UCurveFloat* HunterSpawnMultiplierCurve;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	float CurrBurrowerSpawnMultiplier;
	

};
