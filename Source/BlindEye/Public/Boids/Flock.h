// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "DamageTypes/BaseDamageType.h"
#include "GameFramework/Actor.h"
#include "Tools/LocalPlayerSubsystem_Pooling.h"
#include "Flock.generated.h"

class ABoid;

UCLASS()
class BLINDEYE_API AFlock : public AActor
{
	GENERATED_BODY()
	
public:	

	AFlock();

	UPROPERTY(EditDefaultsOnly)
	EActorPoolType TagPoolType;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABoid> BoidType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int FlockWaveSize = 3;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0))
	int FlockSizeVariation = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int FlockWaveCount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strength")
	float SeparationStrength = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strength")
	float AlignmentStrength = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strength")
	float CohesionStrength = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strength")
	float TargetStrength = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strength")
	float ObstacleStrength = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strength")
	float NoiseStrength = 100.f;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strength")
	float SwirlStrength = 100.f;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NoiseAngleVariation = 25.f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DesiredSeparation = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float XSpawnRange = 150.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float YSpawnRange = 150.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ZSpawnRange = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ObstacleRadius = 1000.f;
	
	TArray<TWeakObjectPtr<AActor>> TargetList;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BoidMaxInitialVertical = 1000.f;

	UPROPERTY(EditDefaultsOnly)
	float DelayBetweenWaveSpawning = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BoidMinDistanceToNotApplyArc = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BoidMaxDistanceToApplyFullArc = 6000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BoidInitialHorizontalChangeMin = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BoidInitialHorizontalChangeMax = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	float DamageRadius = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	float DamageAmount = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UBaseDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	float DamageCooldown = 0.2f;

	UPROPERTY(EditInstanceOnly, Category="Developer")
	bool bSpawnOnBegin = false;

	UPROPERTY(EditDefaultsOnly, Category=ObstacleAvoidance)
	float SphereRadiusCheckObstacleAvoidance = 250.f;

	UPROPERTY(EditDefaultsOnly, Category=ObstacleAvoidance)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesToAvoid;

	UPROPERTY(EditDefaultsOnly)
	bool bFlippedSwirlRotation = false;

	FName BoneSpawnName = "";
	

protected:
	
	float CurrTargetIndex = 0;

	bool IsCurrTargetValid();

	UPROPERTY(EditDefaultsOnly)
	bool bCanAttack = true;
	TArray<ABoid*> BoidsInFlock;

	FTimerHandle FlockSpawnTimerHandle;
	FTimerHandle CanAttackTimerHandle;

	FTimerHandle PerformFlockTimerHandle;
	float PerformFlockDelay = 0.02;

	bool bFlockInitialized = false;

	virtual void BeginPlay() override;

	FVector CalcAveragePosition();

	bool CheckInRangeOfTarget();

	void TryStartFlock(FVector spawnLocation);

	virtual void Destroyed() override;

public:	

	virtual void Tick(float DeltaTime) override;
	
	void InitializeFlock();
	
	void AddBoid(ABoid* newBoid);
	void SpawnBoidRand();
	void RemoveBoids();

	void SetCanAttack();

	virtual FVector Separation(ABoid* boid);
	virtual FVector Cohesion(ABoid* boid);
	virtual FVector Alignment(ABoid* boid);
	virtual FVector TargetSeeking(ABoid* boid);
	virtual FVector ObstacleAvoidance(ABoid* boid);
	virtual FVector Noise(ABoid* boid);
	virtual FVector Swirling(ABoid* boid, FVector AvgPosition, FVector Alignment);

	void UpdateMaxSpeed(float PercentToChangeSpeed);

private:

	int currFlocksSpawned = 0;
	
	void SpawnFlockWave();

	void PerformFlock();

	FVector SpawnLocation;
};
