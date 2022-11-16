// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PooledActorInterface.h"
#include "Boid.generated.h"

class UProjectileMovementComponent;

UCLASS()
class BLINDEYE_API ABoid : public AActor, public IPooledActorInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoid();

	float GetX();
	float GetY();
	float GetZ();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta=(ClampMin=0, ClampMax=90))
	float MaxRollRotation = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta=(ClampMin=0.001, ClampMax=1))
	float RollRotationSpeed = 0.02;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta=(ClampMin=0.001, ClampMax=1))
	float RollRotationToRestingSpeed = 0.01;

	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float MaxRotationAmount = 0.004f;

	UPROPERTY(EditDefaultsOnly)
	float MaxSpeed = 2000;
 
	FVector GetVelocity() const;
	void SetVelocity(FVector& velocity);
	void AddForce(FVector& velocity);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* RootScene;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* BoidMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* BirdMesh;

	UPROPERTY(EditDefaultsOnly, Category=SpawnGrow)
	float TimeUntilFullSizeOnSpawn = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category=SpawnGrow)
	float SpawnScaleSize = 0.2;

	void UpdateMaxSpeed(float PercentOfMaxSpeed);

	void InitializeBoid(FVector Location, FRotator Rotation);

	void DisableActor(bool bDisabled) override;
	virtual bool GetIsActorDisabled() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void FellOutOfWorld(const UDamageType& dmgType) override;

	void InitialSpawnSizeGrow();
	FTimerHandle SpawnSizeGrowTimerHandle;
	float CurrTimerSizeGrow = 0;
	float SizeGrowTimerDelay = 0.02;

	FVector Initial3DScale; 

	float CurrMaxSpeed;

	bool bIsDisabled = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FVector PrevVelocity;

	void HorizontalRotation();

};
