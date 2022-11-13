// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlindEyeBaseCharacter.h"
#include "Enemies/Snapper/SnapperEnemy.h"
#include "Enemies/Burrower/BurrowerEnemy.h"
#include "GameFramework/Character.h"
#include "Gameplay/BlindEyeGameState.h"
#include "HUD/PlayerScreenIndicator.h"
#include "HUD/ScreenIndicator.h"
#include "HUD/W_Radar.h"
#include "Interfaces/AbilityUserInterface.h"
#include "HUD/TextPopupWidget.h"
#include "BlindEyeUtils.h"
#include "BlindEyePlayerCharacter.generated.h"

enum class TEAMS;
enum class EAbilityTypes : uint8;
class UAbilityManager;
class UHealthComponent;
class ABlindEyePlayerState;

class UMarkerComponent;
class UTextPopupManager;

UENUM(BlueprintType)
enum class ETutorialChecklist : uint8
{
	Jump, 
	Dash,
	BasicAttack,
	Combo,
	Ability1,
	Ability2,
	MarkEnemy,
	Detonate, 
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(ETutorialChecklist, ETutorialChecklist::Count);

UCLASS(config=Game)
class ABlindEyePlayerCharacter : public ABlindEyeBaseCharacter, public IAbilityUserInterface, public IIndicatorInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* HealthbarVisibilityBounds;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0, ClampMax=1))
	float HunterMarkMovementAlter = 0;

	UPROPERTY(EditDefaultsOnly)
	float CooldownRefreshAmount = 2.0f;

	UPROPERTY(EditDefaultsOnly)
	float ButtonHoldToSkipTutorial = 3.f;

	UPROPERTY(EditDefaultsOnly)
	float DefendShrineIndicatorLength = 60.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UPlayerScreenIndicator> PlayerIndicatorType;
	
public:
	ABlindEyePlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void FellOutOfWorld(const UDamageType& dmgType) override;

	virtual void OnRep_PlayerState() override;
	virtual void PossessedBy(AController* NewController) override;

	virtual void BeginPlay() override;

	UFUNCTION(Client, Reliable)
	void CLI_TryFinishTutorial(ETutorialChecklist CheckListItem);

	void OnEnemyMarkDetonated();
	UFUNCTION(BlueprintImplementableEvent) 
	void BP_CooldownRefreshed(float RefreshAmount);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateCooldownUI(EAbilityTypes abilityType, float CurrCooldown, float MaxCooldown);

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAbilityManager* AbilityManager;

	virtual float GetHealth() override;
	virtual void SetHealth(float NewHealth) override;
	
	virtual void OnDeath(AActor* ActorThatKilled) override;
	UFUNCTION(NetMulticast, Reliable)
	void MULT_OnDeath(AActor* ActorThatKilled);

	virtual bool TryConsumeBirdMeter(float PercentAmount) override;

	virtual float GetMaxHealth() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=0, ClampMax=100))
	float BirdMeterRegenPercentPerSec = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HealthRegenPerSec = 1.f;  

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ReviveSpeedAllyPercentPerSec = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ReviveSpeedAutoPercentPerSec = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HealthPercentOnRevive = 50;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AllyReviveRadius = 300;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> AllyReviveObjectTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TimerAfterAbilityUsed = 1.0f;

	UPROPERTY(Replicated, BlueprintGetter=GetPlayerType)
	EPlayerType PlayerType;
 
	UFUNCTION(BlueprintPure)
	EPlayerType GetPlayerType(); 

	// Debugger Functionality *********

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SER_DebugInvincibility(bool IsInvincible);
	// Kill Debuggers
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SER_DebugKillAllSnappers();
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SER_DebugKillAllBurrowers();
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SER_DebugKillAllHunters();
	// Spawn Debuggers
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SER_DebugSpawnSnapper();
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SER_DebugSpawnBurrower();
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SER_DebugSpawnHunter();
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SER_DamageSelf(); 
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SER_DamageShrine();
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SER_IncrementTimeByAMinute(); 

	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SER_ShrineInvincibility(bool IsInvincible);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SER_UnlimitedBirdMeter(bool IsUnlimited);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SER_PauseWinCondition(bool IsWinCondPaused);
	UFUNCTION(Server, Reliable, BlueprintCallable) 
	void SER_HunterAlwaysVisible(bool IsHunterAlwaysVisible);   

	UFUNCTION(BlueprintCallable)
	bool GetIsInvincible();
	UFUNCTION(BlueprintCallable)
	bool GetIsShrineInvincible();
	UFUNCTION(BlueprintCallable)
	bool GetIsUnlimitedBirdMeter();
	UFUNCTION(BlueprintCallable) 
	bool GetIsWinConditionPaused();
	UFUNCTION(BlueprintCallable) 
	bool GetIsHunterAlwaysVisible();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ASnapperEnemy> SnapperType;

	// End Debugger Functionality *********

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
	float GetBirdMeterPercent() override;
	virtual float GetBirdMeter() override;

	void OnGameLost();
	void OnGameWon();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateShrineHealthUI(); 
	float GetHealthPercent() override;

	virtual bool GetIsDead() override;
	
	bool IsActionsBlocked();

	UFUNCTION(BlueprintCallable)
	float GetAllyHealthPercent();

	UFUNCTION(BlueprintCallable)
	float GetShrineHealthPercent();  

	// Called when playerState replicated on client
	void UpdateAllClientUI();

	// Locks the player direction to controller rotation.
	// If Duration = 0, keeps rotational lock until manually turned off
	UFUNCTION(NetMulticast, Reliable)
	void MULT_StartLockRotationToController(float Duration);

	// Stops the rotational lock to controller and clears timer
	UFUNCTION(Client, Reliable)
	void CLI_StopLockRotationToController();

	UAbilityManager* GetAbilityManager();

	UFUNCTION(NetMulticast, Reliable)
	void MULT_PlayAnimMontage(UAnimMontage* AnimMontage);

	UFUNCTION(NetMulticast, Reliable) 
	void MULT_StopAnimMontage(UAnimMontage* AnimMontage); 

	UFUNCTION(NetMulticast, Reliable)  
	void MULT_SetNextMontageSection(UAnimMontage* AnimMontage, FName Section);


	UFUNCTION(Client, Reliable) 
	void CLI_UpdateRotationRate(float NewRotationRate);
	UFUNCTION(Client, Reliable)
	void CLI_ResetRotationRateToNormal();
 
	UFUNCTION(NetMulticast, Reliable)  
	void MULT_UpdateWalkMovementSpeed(float PercentWalkSpeedChange, float PercentAccelerationChange);
	UFUNCTION(NetMulticast, Reliable)  
	void MULT_ResetWalkMovementToNormal();

	// Entrance method when tutorial started
	UFUNCTION()
	void StartTutorial();

	// Entrance method for when main game loop starts
	UFUNCTION()
	void StartGame(); 
 
	UFUNCTION(BlueprintImplementableEvent)
	void BP_DisplayTutorialChecklist_CLI(bool bShowChecklist);

	UFUNCTION()
	void HealthbarBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void HealthbarEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Event for when another player died, called from GameState
	void OnOtherPlayerDied(ABlindEyePlayerCharacter* OtherPlayer); 
	
	// Event for when another player revived, called from GameState
	void OnOtherPlayerRevived(ABlindEyePlayerCharacter* OtherPlayer);

	UFUNCTION(NetMulticast, Reliable)
	void MULT_OnMarked(AActor* MarkedPlayer, EMarkerType MarkType); 
	UFUNCTION(NetMulticast, Reliable) 
	void MULT_OnUnMarked(AActor* UnMarkedPlayer, EMarkerType MarkType);

	// Notify the owning client that another player was hunter marked/unmarked
	void NotifyOtherPlayerHunterMarked();
	void NotifyOtherPlayerHunterUnMarked();

	void NotifyOfOtherPlayerExistance(ABlindEyePlayerCharacter* NewPlayer);

	virtual FVector GetIndicatorPosition() override;

	UFUNCTION(Client, Reliable)
	void CLI_AddEnemyTutorialTextSnippet(EEnemyTutorialType EnemyTutorialType);
	UFUNCTION(BlueprintImplementableEvent)
	void BP_AddEnemyTutorialText(EEnemyTutorialType EnemyTutorialType);
	UFUNCTION(Client, Reliable) 
	void CLI_RemoveEnemyTutorialTextSnippet();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_RemoveEnemyTutorialText();

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void CLI_AddTextPopup(const FString& Text, ETextPopupType TextPopupType, float Duration = 0);

	UPROPERTY(Replicated)
	FTutorialActionBlockers TutorialActionBlockers;

	 
protected:

	TSet<ETutorialChecklist> ChecklistFinishedTasks;
	bool bTutorial1Finished = false;

	UFUNCTION()
	void RegenBirdMeter();
	const float RegenBirdMeterCallDelay = 0.2f;	// Delay on timer call for regen-ing bird meter
	FTimerHandle BirdRegenTimerHandle;

	UFUNCTION()
	void RegenHealth(); 
	const float RegenHealthCallDelay = 0.2f;	// Delay on timer call for regen-ing bird meter
	FTimerHandle HealthRegenTimerHandle;

	UPROPERTY(Replicated)
	bool bUnlimitedBirdMeter = false;

	FTimerHandle RotationalLockTimerHandle;

	float CachedRotationRate;

	float CachedMovementSpeed;
	float CachedAcceleration;

	UPROPERTY(EditAnywhere) 
	UPlayerScreenIndicator* PlayerIndicator;

	// Crated in BP, holds manager for dealing with all text popups
	UPROPERTY(BlueprintReadWrite)
	UTextPopupManager* TextPopupManager;

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

	UFUNCTION(BlueprintImplementableEvent)
	void OnGameLostUI();
	UFUNCTION(BlueprintImplementableEvent)
	void OnGameWonUI();
	
	void OnCheckAllyHealing();
	FTimerHandle AllyHealingCheckTimerHandle;
	const float AllyHealCheckDelay = 0.2f;

	UPROPERTY(Replicated, EditDefaultsOnly)
	float CurrRevivePercent = 0; 

	UFUNCTION(BlueprintImplementableEvent)
	void BP_TutorialCheckList(ETutorialChecklist TutorialChecklist);
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_PlayerRevived();
 
	UFUNCTION(BlueprintImplementableEvent)
	void BP_RevivePercentUpdate(float RevivePercent);
	
	void OnRevive();

	UFUNCTION()
	void BasicAttackPressed();
	UFUNCTION()
	void ChargedAttackPressed();
	UFUNCTION()
	void ChargedAttackReleased();

	UFUNCTION() 
	void TryJump();

	UFUNCTION()  
	void DashPressed();
	UFUNCTION() 
	void DashReleased(); 

	UFUNCTION()
	void Unique1Pressed();
	UFUNCTION()
	void Unique1Released();

	UFUNCTION()
	void Unique2Pressed();
	UFUNCTION()
	void Unique2Released();

	UFUNCTION(Server, Reliable)
	void SER_SetTutorialFinished();

	void TutorialSkipPressed();
	void TutorialSkipReleased();

	void UserSkipTutorial();
	UFUNCTION(Server, Reliable)
	void SER_UserInputSkipTutorial();
	FTimerHandle TutorialSkipTimerHandle;

	FTimerHandle RadarUpdateTimerHandle;
	float RadarUpdateDelay = 0.15f;
	UFUNCTION()
	void UpdateRadar();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_DisplayDefendShrineIndicator_CLI(bool bShowIndicator);
	UFUNCTION(NetMulticast, Reliable)
	void MULT_HideDefendShrineIndicator();
	FTimerHandle HideDefendShrineIndicatorTimerHandle;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	ABlindEyePlayerState* GetAllyPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

