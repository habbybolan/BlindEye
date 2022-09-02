// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HealthInterface.h"
#include "BlindEyeEnemyBase.generated.h"

UCLASS()
class BLINDEYE_API ABlindEyeEnemyBase : public ACharacter, public IHealthInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABlindEyeEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHealth = 100.f;
	UPROPERTY(Replicated)
	float CurrHealth;

	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float GetHealth_Implementation() override;
	virtual void SetHealth_Implementation(float NewHealth) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEAMS Team;
	virtual TEAMS GetTeam_Implementation() override;

};
