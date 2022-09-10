// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlindEyePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API ABlindEyePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ABlindEyePlayerState();

	float GetHealth();
	void SetHealth(float NewHealth);
	float GetMaxHealth();

	float GetBirdMeter();
	void SetBirdMeter(float NewBirdMeter);

	DECLARE_MULTICAST_DELEGATE(FHealthUpdatedSiganture)
	FHealthUpdatedSiganture HealthUpdated;

protected:
	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxHealth = 100.f;
	UPROPERTY(Replicated, ReplicatedUsing="OnRep_HealthUpdated")
	float CurrHealth;

	UFUNCTION()
	void OnRep_HealthUpdated();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxBirdMeter = 100.f;
	UPROPERTY(Replicated, ReplicatedUsing="OnRep_BirdMeterUpdated")
	float CurrBirdMeter;

	UFUNCTION()
	void OnRep_BirdMeterUpdated();
	
	
};
