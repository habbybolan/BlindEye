// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhoenixFireballCast.generated.h"


class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class BLINDEYE_API APhoenixFireballCast : public AActor
{
	GENERATED_BODY()
	
public:
	
	APhoenixFireballCast();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FireballSpeed = 100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Damage = 60;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BurningDuration = 4.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BurningRadius = 400.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHeightToApplyFire;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* FireTrailParticle;

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY()
	UNiagaraComponent* SpawnedFireTrailParticle;

};
