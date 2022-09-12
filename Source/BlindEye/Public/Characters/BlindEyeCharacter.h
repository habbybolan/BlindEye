// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/AbilityUserInterface.h"
#include "Interfaces/HealthInterface.h"
#include "BlindEyeCharacter.generated.h"

enum class TEAMS;
class UAbilityManager;
class UHealthComponent;
class ABlindEyePlayerState;

UENUM(BlueprintType)
enum class PlayerType : uint8
{
	CrowPlayer,
	PhoenixPlayer
};

UCLASS(config=Game)
class ABlindEyeCharacter : public ACharacter, public IHealthInterface, public IAbilityUserInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
public:
	ABlindEyeCharacter();

	virtual void OnRep_PlayerState() override;
	virtual void PossessedBy(AController* NewController) override;

	virtual void BeginPlay() override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAbilityManager* AbilityManager;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UHealthComponent* HealthComponent;

	virtual float GetHealth_Implementation() override;
	virtual void SetHealth_Implementation(float NewHealth) override;

	virtual void OnTakeDamage_Implementation(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser) override;
	virtual UHealthComponent* GetHealthComponent_Implementation() override;

	virtual void OnDeath_Implementation() override;

	virtual bool TryConsumeBirdMeter_Implementation(float PercentAmount) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0, ClampMax=100))
	float BirdMeterRegenPercentPerSec = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HealthRegenPerSec = 1.f;  

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEAMS Team;
	virtual TEAMS GetTeam_Implementation() override;

	// Event called after playerState updates health
	UFUNCTION()
	void HealthUpdated();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdatePlayerHealthUI();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateAllyHealthUI();

	// Event called after PlayerState updated Bird meter
	UFUNCTION()
	void BirdMeterUpdated(); 
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateBirdMeterUI();
	float GetBirdMeterPercent_Implementation() override;
	virtual float GetBirdMeter_Implementation() override;
	

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateShrineHealthUI(); 
	float GetHealthPercent_Implementation() override;

	virtual bool GetIsDead_Implementation() override;

	UFUNCTION(BlueprintCallable)
	float GetAllyHealthPercent();

	UFUNCTION(BlueprintCallable)
	float GetShrineHealthPercent();  

	// Called when playerState replicated on client
	void UpdateAllClientUI();

	PlayerType PlayerType;

protected:

	UFUNCTION()
	void RegenBirdMeter();
	const float RegenBirdMeterCallDelay = 0.2f;	// Delay on timer call for regen-ing bird meter
	FTimerHandle BirdRegenTimerHandle;

	UFUNCTION()
	void RegenHealth(); 
	const float RegenHealthCallDelay = 0.2f;	// Delay on timer call for regen-ing bird meter
	FTimerHandle HealthRegenTimerHandle;  
	
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	UFUNCTION()
	void BasicAttackPressed();
	UFUNCTION()
	void ChargedAttackPressed();
	UFUNCTION()
	void ChargedAttackReleased(); 

	UFUNCTION()
	void Unique1Pressed();
	UFUNCTION()
	void Unique1Released();

	UFUNCTION()
	void Unique2Pressed();
	UFUNCTION()
	void Unique2Released();

	UFUNCTION(Server, Reliable)
	void SER_DebugInvincibility();
	UFUNCTION(Server, Reliable)
	void SER_DebugKillAllSnappers();
	UFUNCTION(Server, Reliable)
	void SER_DebugKillAllBurrowers();
	UFUNCTION(Server, Reliable)
	void SER_DebugKillAllHunters();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	ABlindEyePlayerState* GetAllyPlayerState();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

