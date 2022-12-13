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
	float GetMaxBirdMeter();
	void SetBirdMeter(float NewBirdMeter);
	
	bool GetIsDead();
	void SetIsDead(bool IsDead);

	bool GetIsTutorialFinished();
	void SetTutorialFinished(bool IsTutorialFinished);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerDeathSignature, ABlindEyePlayerState*, PlayerState);
	FPlayerDeathSignature PlayerDeathDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerRevivedSignature, ABlindEyePlayerState*, PlayerState);
	FPlayerRevivedSignature PlayerRevivedDelegate;

	UPROPERTY(Replicated)
	bool bActionsBlocked = false;

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
 
	UPROPERTY(Replicated, ReplicatedUsing="OnRep_PlayerDeathStateUpdated")
	bool IsDead = false;

	UFUNCTION()
	void OnRep_PlayerDeathStateUpdated();

	UPROPERTY(Replicated)
	bool bFinishedTutorial = false;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_PlayerFinishedTutorial_SER();

	UFUNCTION()
	void OnRep_BirdMeterUpdated();
	
	
};
