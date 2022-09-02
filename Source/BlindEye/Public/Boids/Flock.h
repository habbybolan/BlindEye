// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Flock.generated.h"

class ABoid;
class UBaseDamageType;

UCLASS()
class BLINDEYE_API AFlock : public AActor
{
	GENERATED_BODY()
	
public:	

	AFlock();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABoid> BoidType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int FlockWaveSizeMax = 3;

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

	UPROPERTY(replicated, EditInstanceOnly, ReplicatedUsing = OnRep_Target)
	TWeakObjectPtr<AActor> Target;

	UFUNCTION()
	void OnRep_Target();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BoidMaxInitialVertical = 1000.f;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UBaseDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	float DamageCooldown = 0.2f;

	UPROPERTY(EditInstanceOnly, Category="Developer")
	bool bSpawnOnBegin = false;

protected:

	bool bCanAttack = true;

	FTimerHandle FlockSpawnTimerHandle;
	FTimerHandle CanAttackTimerHandle;

	virtual void BeginPlay() override;

	FVector CalcAveragePosition();

	bool CheckInRangeOfTarget();

	virtual void Destroyed() override;

public:	

	virtual void Tick(float DeltaTime) override;


	void InitializeFlock();
	
	void AddBoid(ABoid* newBoid);
	void SpawnBoidRand();
	void RemoveBoids();

	void SetCanAttack();

private:
	
	TArray<class UObstacle*> AllObstacles;

	int currFlocksSpawned = 0;
	
	TArray<ABoid*> BoidsInFlock;
	void SpawnFlockWave();

	FVector Separation(ABoid* boid);
	FVector Cohesion(ABoid* boid);
	FVector Alignment(ABoid* boid);

	FVector TargetSeeking(ABoid* boid);

	UFUNCTION(NetMulticast, Unreliable)
	void PerformFlock();
};
