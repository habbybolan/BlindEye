// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Boid.generated.h"

class UProjectileMovementComponent;

UCLASS()
class BLINDEYE_API ABoid : public AActor
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
 
	FVector GetVelocity() const;
	void SetVelocity(FVector& velocity);
	void AddForce(FVector& velocity);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* RootScene;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* BoidMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, Category=SpawnGrow)
	float TimeUntilFullSizeOnSpawn = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category=SpawnGrow)
	float SpawnScaleSize = 0.2;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitialSpawnSizeGrow();
	FTimerHandle SpawnSizeGrowTimerHandle;
	float CurrTimerSizeGrow = 0;
	float SizeGrowTimerDelay = 0.02;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FVector PrevVelocity;

	void HorizontalRotation();

};
