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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FVector PrevVelocity;

	void HorizontalRotation();

};
