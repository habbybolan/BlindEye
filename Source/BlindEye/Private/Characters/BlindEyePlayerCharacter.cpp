// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/BlindEyePlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Abilities/AbilityManager.h"
#include "Characters/BlindEyePlayerController.h"
#include "Components/HealthComponent.h"
#include "Components/MarkerComponent.h"
#include "DamageTypes/DebugDamageType.h"
#include "Enemies/BlindEyeEnemyController.h"
#include "Enemies/Burrower/BurrowerEnemy.h"
#include "Enemies/Snapper/SnapperEnemy.h"
#include "GameFramework/PlayerStart.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Gameplay/BlindEyePlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "BlindEyeUtils.h"
#include "Enemies/Hunter/HunterEnemy.h"
#include "Enemies/Hunter/HunterEnemyController.h"
#include "Enemies/Burrower/BurrowerSpawnManager.h"
#include "Enemies/Burrower/BurrowerSpawnPoint.h"
#include "Gameplay/BlindEyeGameMode.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

//////////////////////////////////////////////////////////////////////////
// ATP_ThirdPersonCharacter

ABlindEyePlayerCharacter::ABlindEyePlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	AbilityManager = CreateDefaultSubobject<UAbilityManager>(TEXT("AbilityManager"));
	
	PlayerType = EPlayerType::CrowPlayer;
	Team = TEAMS::Player;
}

void ABlindEyePlayerCharacter::FellOutOfWorld(const UDamageType& dmgType)
{
	// reset player to a playerStart on killz reached
	if (GetLocalRole() == ROLE_Authority)
	{
		UWorld* world = GetWorld();
		if (world == nullptr) return;

		AActor* ActorPlayerStart = UGameplayStatics::GetActorOfClass(world, APlayerStart::StaticClass());
		SetActorLocation(ActorPlayerStart->GetActorLocation());
	}
}

void ABlindEyePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	UWorld* world = GetWorld();

	CachedRotationRate = GetCharacterMovement()->RotationRate.Yaw;
	CachedMovementSpeed = GetCharacterMovement()->MaxWalkSpeed;
	CachedAcceleration = GetCharacterMovement()->MaxAcceleration;
	
	if (world == nullptr) return;

	if (IsLocallyControlled())
	{
		AActor* ShrineActor = UGameplayStatics::GetActorOfClass(world, AShrine::StaticClass());
		if (ShrineActor)
		{
			AShrine* Shrine = Cast<AShrine>(ShrineActor);
			Shrine->ShrineHealthChange.AddUFunction(this, TEXT("UpdateShrineHealthUI"));
		}
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		world->GetTimerManager().SetTimer(BirdRegenTimerHandle, this, &ABlindEyePlayerCharacter::RegenBirdMeter, RegenBirdMeterCallDelay, true);
		world->GetTimerManager().SetTimer(HealthRegenTimerHandle, this, &ABlindEyePlayerCharacter::RegenHealth, RegenHealthCallDelay, true);
	}

	ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(world));
	check(BlindEyeGS);

	// Check if state is Tutorial
	if (BlindEyeGS->IsBlindEyeMatchTutorial())
	{
		StartTutorial();
	}
	// Check if waiting for players, subscribe to tutorial starting event
	else 
	{
		BlindEyeGS->TutorialStartedDelegate.AddDynamic(this, &ABlindEyePlayerCharacter::StartTutorial);
	}
}

void ABlindEyePlayerCharacter::TryFinishTutorial(ETutorialChecklist CheckListItem)
{
	// Check if Player in tutorial
	UWorld* World = GetWorld();
	if (World)
	{
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
		if (BlindEyeGS->IsBlindEyeMatchTutorial())
		{
			if ((!bTutorial1Finished && CheckListItem <= ETutorialChecklist::Ability2) ||
				(bTutorial1Finished && CheckListItem > ETutorialChecklist::Ability2))
			// Check if checklist item not yet done
			if (!ChecklistFinishedTasks.Contains(CheckListItem))
			{
				// Send checklist item to BP
				BP_TutorialCheckList(CheckListItem);
				ChecklistFinishedTasks.Add(CheckListItem);
				
				// Swap to next tutorial if finished first
				if (!bTutorial1Finished && ChecklistFinishedTasks.Num() > 6)
				{
					bTutorial1Finished = !bTutorial1Finished;
				}
			}
		}
	}
}

void ABlindEyePlayerCharacter::OnEnemyMarkDetonated()
{
	AbilityManager->RefreshAllCooldowns(CooldownRefreshAmount);
}

void ABlindEyePlayerCharacter::StartTutorial()
{
	BP_DisplayTutorialChecklist(true);
	UWorld* World = GetWorld();
	if (World)
	{
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
		check(BlindEyeGS)
		BlindEyeGS->GameStartedDelegate.AddDynamic(this, &ABlindEyePlayerCharacter::StartGame);
	}
}

void ABlindEyePlayerCharacter::StartGame()
{
	BP_DisplayTutorialChecklist(false);
}

ABlindEyePlayerState* ABlindEyePlayerCharacter::GetAllyPlayerState()
{
	ABlindEyeGameState* GameState = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (!GameState) return nullptr;

	// Get other player controller
	ABlindEyeGameState* BlindEyeGameState = Cast<ABlindEyeGameState>(GetWorld()->GetGameState());
	if (BlindEyeGameState == nullptr) return nullptr;

	if (BlindEyeGameState->PlayerArray.Num() < 0) return nullptr;
	APlayerState* PlayerState1 = BlindEyeGameState->PlayerArray[0];
	if (PlayerState1 && PlayerState1 != GetPlayerState())
	{
		return Cast<ABlindEyePlayerState>(PlayerState1);
	} else if (APlayerState* PlayerState2 = BlindEyeGameState->PlayerArray[1])
	{
		return Cast<ABlindEyePlayerState>(PlayerState2);
	}
	return nullptr;
}

void ABlindEyePlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (IsLocallyControlled())
	{
		UpdateAllClientUI();
	}
}

void ABlindEyePlayerCharacter::UpdateAllClientUI()
{
	UpdatePlayerHealthUI();
}

void ABlindEyePlayerCharacter::MULT_StartLockRotationToController_Implementation(float Duration)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return; 

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Override current timer duration if new duration larger
	float TimeRemaining = World->GetTimerManager().GetTimerRemaining(RotationalLockTimerHandle);
	if (Duration + TimerAfterAbilityUsed > TimeRemaining)
	{
		World->GetTimerManager().SetTimer(RotationalLockTimerHandle, this, &ABlindEyePlayerCharacter::CLI_StopLockRotationToController,
			Duration +  TimerAfterAbilityUsed, false);
	} 
}

void ABlindEyePlayerCharacter::CLI_StopLockRotationToController_Implementation()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	World->GetTimerManager().ClearTimer(RotationalLockTimerHandle);
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

UAbilityManager* ABlindEyePlayerCharacter::GetAbilityManager()
{
	return AbilityManager;
}

void ABlindEyePlayerCharacter::MULT_PlayAnimMontage_Implementation(UAnimMontage* AnimMontage)
{
	PlayAnimMontage(AnimMontage); 
}

void ABlindEyePlayerCharacter::MULT_StopAnimMontage_Implementation(UAnimMontage* AnimMontage)
{
	StopAnimMontage(AnimMontage);
}

void ABlindEyePlayerCharacter::MULT_SetNextMontageSection_Implementation(UAnimMontage* AnimMontage, FName Section)
{
	GetMesh()->GetAnimInstance()->Montage_JumpToSection(Section, AnimMontage);
}

void ABlindEyePlayerCharacter::CLI_UpdateRotationRate_Implementation(float NewRotationRate)
{
	GetCharacterMovement()->RotationRate = FRotator(0, NewRotationRate, 0);
}

void ABlindEyePlayerCharacter::CLI_ResetRotationRateToNormal_Implementation()
{
	GetCharacterMovement()->RotationRate = FRotator(0, CachedRotationRate, 0);
}

void ABlindEyePlayerCharacter::MULT_UpdateWalkMovementSpeed_Implementation(float PercentWalkSpeedChange, float PercentAccelerationChange)
{
	GetCharacterMovement()->MaxWalkSpeed = CachedMovementSpeed * PercentWalkSpeedChange;
	GetCharacterMovement()->MaxAcceleration = CachedAcceleration * PercentAccelerationChange;
}

void ABlindEyePlayerCharacter::MULT_ResetWalkMovementToNormal_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = CachedMovementSpeed;
	GetCharacterMovement()->MaxAcceleration = CachedAcceleration;
}

void ABlindEyePlayerCharacter::TutorialFinished()
{
	// TODO: Add ready-up button hold before sending notify GM tutorial finished
	UWorld* World = GetWorld();
	if (World)
	{
		ABlindEyeGameMode* BlindEyeGM = Cast<ABlindEyeGameMode>(UGameplayStatics::GetGameMode(World));
		BlindEyeGM->TutorialFinished(this);
	}
	
}

void ABlindEyePlayerCharacter::RegenBirdMeter()
{
	if (ABlindEyePlayerState* BlindEyePlayerState = Cast<ABlindEyePlayerState>(GetPlayerState()))
	{ 
		float BirdMeterIncrPerSec = BlindEyePlayerState->GetMaxBirdMeter() * (BirdMeterRegenPercentPerSec / 100);
		float NewBirdMeter = FMath::Min(BlindEyePlayerState->GetBirdMeter() + (BirdMeterIncrPerSec * RegenBirdMeterCallDelay),
			BlindEyePlayerState->GetMaxBirdMeter());
		BlindEyePlayerState->SetBirdMeter(NewBirdMeter);
	}
}

void ABlindEyePlayerCharacter::RegenHealth()
{
	if (ABlindEyePlayerState* BlindEyePlayerState = Cast<ABlindEyePlayerState>(GetPlayerState()))
	{ 
		float NewHealth = FMath::Min(BlindEyePlayerState->GetHealth() + (HealthRegenPerSec * RegenHealthCallDelay),
			BlindEyePlayerState->GetMaxHealth()); 
		BlindEyePlayerState->SetHealth(NewHealth);
	}
}

void ABlindEyePlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ABlindEyePlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABlindEyePlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
 
void ABlindEyePlayerCharacter::SER_OnCheckAllyHealing_Implementation()
{
	// TODO: Sphere cast around on timer for healing
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	TArray<AActor*> OverlapActors;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), AllyReviveRadius, AllyReviveObjectTypes,
		nullptr, ActorsToIgnore, OverlapActors);
	if (OverlapActors.Num() > 0)
	{
		CurrRevivePercent += AllyHealCheckDelay * ReviveSpeedAllyPercentPerSec;
	} else
	{
		CurrRevivePercent += AllyHealCheckDelay * ReviveSpeedAutoPercentPerSec;
	}
	
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.2f, FColor::Green, "Revive %: " + FString::SanitizeFloat(CurrRevivePercent));
	if (CurrRevivePercent >= 100)
	{
		SER_OnRevive();
	}
}

void ABlindEyePlayerCharacter::SER_OnRevive_Implementation()
{ 
	if (ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(GetPlayerState()))
	{
		BlindEyePS->SetHealth(BlindEyePS->GetMaxHealth() * HealthPercentOnRevive);
		BlindEyePS->SetIsDead(false);
		GetWorldTimerManager().ClearTimer(AllyHealingCheckTimerHandle);
		CurrRevivePercent = 0;
	}
}

void ABlindEyePlayerCharacter::BasicAttackPressed() 
{
	TryFinishTutorial(ETutorialChecklist::BasicAttack);
	if (IsActionsBlocked()) return;
	AbilityManager->SER_UsedAbility(EAbilityTypes::Basic, EAbilityInputTypes::Pressed);
}

void ABlindEyePlayerCharacter::ChargedAttackPressed()
{
	if (IsActionsBlocked()) return;
	AbilityManager->SER_UsedAbility(EAbilityTypes::ChargedBasic, EAbilityInputTypes::Pressed);
}

void ABlindEyePlayerCharacter::ChargedAttackReleased()
{
	if (IsActionsBlocked()) return;
	AbilityManager->SER_UsedAbility(EAbilityTypes::ChargedBasic, EAbilityInputTypes::Released);
}

void ABlindEyePlayerCharacter::DashPressed()
{
	TryFinishTutorial(ETutorialChecklist::Dash);
	if (IsActionsBlocked()) return;
	AbilityManager->SER_UsedAbility(EAbilityTypes::Dash, EAbilityInputTypes::Pressed);
}

void ABlindEyePlayerCharacter::DashReleased()
{
	if (IsActionsBlocked()) return;
	AbilityManager->SER_UsedAbility(EAbilityTypes::ChargedBasic, EAbilityInputTypes::Released);
}

void ABlindEyePlayerCharacter::Unique1Pressed()
{
	TryFinishTutorial(ETutorialChecklist::Ability1);
	if (IsActionsBlocked()) return;
	AbilityManager->SER_UsedAbility(EAbilityTypes::Unique1, EAbilityInputTypes::Pressed);
}

void ABlindEyePlayerCharacter::Unique1Released()
{
	if (IsActionsBlocked()) return;
	AbilityManager->SER_UsedAbility(EAbilityTypes::Unique1, EAbilityInputTypes::Released);
}

void ABlindEyePlayerCharacter::Unique2Pressed()
{
	TryFinishTutorial(ETutorialChecklist::Ability2);
	if (IsActionsBlocked()) return;
	AbilityManager->SER_UsedAbility(EAbilityTypes::Unique2, EAbilityInputTypes::Pressed);
}

void ABlindEyePlayerCharacter::Unique2Released()
{
	if (IsActionsBlocked()) return;
	AbilityManager->SER_UsedAbility(EAbilityTypes::Unique2, EAbilityInputTypes::Released);
}

void ABlindEyePlayerCharacter::SER_DebugInvincibility_Implementation(bool IsInvincible)
{
	HealthComponent->IsInvincible = IsInvincible;
}

void ABlindEyePlayerCharacter::SER_DebugKillAllSnappers_Implementation()
{
	TArray<AActor*> SnapperActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASnapperEnemy::StaticClass(), SnapperActors);
	for (AActor* SnapperActor : SnapperActors)
	{
		if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(SnapperActor))
		{
			HealthInterface->GetHealthComponent()->Kill();
		}
	}
}

void ABlindEyePlayerCharacter::SER_DebugKillAllBurrowers_Implementation()
{
	TArray<AActor*> BurrowerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABurrowerEnemy::StaticClass(), BurrowerActors);
	for (AActor* BurrowerActor : BurrowerActors)
	{
		if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(BurrowerActor))
		{
			HealthInterface->GetHealthComponent()->Kill();
		}
	}
}

void ABlindEyePlayerCharacter::SER_DebugKillAllHunters_Implementation()
{
	TArray<AActor*> HunterActors; 
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHunterEnemy::StaticClass(), HunterActors);
	for (AActor* HunterActor : HunterActors)
	{
		if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(HunterActor))
		{
			HealthInterface->GetHealthComponent()->Kill();
		}
	}
}

void ABlindEyePlayerCharacter::SER_DebugSpawnSnapper_Implementation()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	TArray<AActor*> BurrowerSpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(World, UBurrowerSpawnPoint::StaticClass(), BurrowerSpawnPoints);
	if (BurrowerSpawnPoints.Num() == 0) return;
	
	UBurrowerSpawnPoint* BurrSpawnPoint = Cast<UBurrowerSpawnPoint>(BurrowerSpawnPoints[UKismetMathLibrary::RandomIntegerInRange(0, BurrowerSpawnPoints.Num() - 1)]);
	if (BurrSpawnPoint)
	{
		World->SpawnActor<ASnapperEnemy>(SnapperType, BurrSpawnPoint->GetComponentLocation(), BurrSpawnPoint->GetComponentRotation());
	}
}

void ABlindEyePlayerCharacter::SER_DebugSpawnBurrower_Implementation()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	if (AActor* BurrowerManager = UGameplayStatics::GetActorOfClass(World, ABurrowerSpawnManager::StaticClass()))
	{
		ABurrowerSpawnManager* HunterController = Cast<ABurrowerSpawnManager>(BurrowerManager);
		HunterController->SpawnBurrower();
	}
}

void ABlindEyePlayerCharacter::SER_DebugSpawnHunter_Implementation()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	if (AActor* HunterActorController = UGameplayStatics::GetActorOfClass(World, AHunterEnemyController::StaticClass()))
	{
		AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(HunterActorController);
		HunterController->DebugSpawnHunter();
	}
}

void ABlindEyePlayerCharacter::SER_DamageSelf_Implementation()
{
	UGameplayStatics::ApplyPointDamage(this, 10, FVector::ZeroVector, FHitResult(),
		GetController(), this, UDebugDamageType::StaticClass());
}

void ABlindEyePlayerCharacter::SER_DamageShrine_Implementation()
{
	if (ABlindEyeGameState* BlindEyeGameState = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(GetWorld())))
	{
		if (BlindEyeGameState->GetShrine())
		{
			UGameplayStatics::ApplyPointDamage(BlindEyeGameState->GetShrine(), 10, FVector::ZeroVector, FHitResult(),
			GetController(), this, UDebugDamageType::StaticClass());
		}
	}
}

void ABlindEyePlayerCharacter::SER_ShrineInvincibility_Implementation(bool IsInvincible)
{
	if (ABlindEyeGameState* BlindEyeGameState = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(GetWorld())))
	{
		AShrine* Shrine = BlindEyeGameState->GetShrine();
		if (Shrine != nullptr)
		{
			if (IHealthInterface* ShrineHealthInterface = Cast<IHealthInterface>(Shrine))
			{
				UHealthComponent* ShrineHealthComp = ShrineHealthInterface->GetHealthComponent();
				if (ShrineHealthComp == nullptr) return;
				ShrineHealthComp->IsInvincible = IsInvincible;
			}
		}
	}
}

void ABlindEyePlayerCharacter::SER_UnlimitedBirdMeter_Implementation(bool IsUnlimited)
{
	bUnlimitedBirdMeter = IsUnlimited;
}

float ABlindEyePlayerCharacter::GetHealth()
{
	if (ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(GetPlayerState()))
		return BlindEyePS->GetHealth();
	return 0;
}

void ABlindEyePlayerCharacter::SetHealth(float NewHealth)
{
	if (ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(GetPlayerState()))
	{
		return BlindEyePS->SetHealth(NewHealth);
	}
}

void ABlindEyePlayerCharacter::OnDeath(AActor* ActorThatKilled) 
{ 
	Super::OnDeath(ActorThatKilled);
	if (ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(GetPlayerState()))
	{
		BlindEyePS->SetIsDead(true);
	}

	GetWorldTimerManager().SetTimer(AllyHealingCheckTimerHandle, this, &ABlindEyePlayerCharacter::SER_OnCheckAllyHealing, AllyHealCheckDelay, true);
}

bool ABlindEyePlayerCharacter::TryConsumeBirdMeter(float PercentAmount)
{
	// Debugger for unlimited bird meter
	if (bUnlimitedBirdMeter) return true;
	
	ABlindEyePlayerState* BlindEyePlayerState = Cast<ABlindEyePlayerState>(GetPlayerState());
	if (BlindEyePlayerState == nullptr) return false;

	// Try to consume bird meter, return true if enough remaining
	float GetExactAmount = BlindEyePlayerState->GetMaxBirdMeter() * (PercentAmount / 100);
	float RemainingMeter = BlindEyePlayerState->GetBirdMeter();
	if (RemainingMeter >= GetExactAmount)
	{
		BlindEyePlayerState->SetBirdMeter(RemainingMeter - GetExactAmount);
		return true;
	}
	return false;
}

float ABlindEyePlayerCharacter::GetMaxHealth()
{
	if (ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(GetPlayerState()))
		return BlindEyePS->GetMaxHealth();
	return 0;
}

EPlayerType ABlindEyePlayerCharacter::GetPlayerType()
{
	return PlayerType;
}

void ABlindEyePlayerCharacter::SER_PauseWinCondition_Implementation(bool IsWinCondPaused)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(World);
	if (ABlindEyeGameMode* BlindEyeGM = Cast<ABlindEyeGameMode>(GameMode))
	{
		BlindEyeGM->PauseWinCondition(IsWinCondPaused);
	}
}


void ABlindEyePlayerCharacter::SER_HunterAlwaysVisible_Implementation(bool IsHunterAlwaysVisible)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	if (AActor* HunterActorController = UGameplayStatics::GetActorOfClass(World, AHunterEnemyController::StaticClass()))
	{
		AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(HunterActorController);
		HunterController->SetAlwaysVisible(IsHunterAlwaysVisible);
	}
}

void ABlindEyePlayerCharacter::SER_IncrementTimeByAMinute_Implementation()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	ABlindEyeGameMode* BlindEyeGameMode = Cast<ABlindEyeGameMode>(UGameplayStatics::GetGameMode(World));
	BlindEyeGameMode->IncrementTimeByAMinute();
}

bool ABlindEyePlayerCharacter::GetIsInvincible()
{
	return HealthComponent->IsInvincible;
}

bool ABlindEyePlayerCharacter::GetIsShrineInvincible()
{
	if (ABlindEyeGameState* BlindEyeGameState = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(GetWorld())))
	{
		AShrine* Shrine = BlindEyeGameState->GetShrine();
		if (Shrine == nullptr) return false;
		if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(Shrine))
		{
			return HealthInterface->GetHealthComponent()->IsInvincible;
		}
	}
	return false;
}

bool ABlindEyePlayerCharacter::GetIsUnlimitedBirdMeter()
{
	return bUnlimitedBirdMeter;
}

bool ABlindEyePlayerCharacter::GetIsWinConditionPaused()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return false;
	
	AGameStateBase* GameState = UGameplayStatics::GetGameState(World);
	if (ABlindEyeGameState* BlindEyeGameState = Cast<ABlindEyeGameState>(GameState))
	{ 
		return BlindEyeGameState->bWinConditionPaused;
	}
	return false;
}

bool ABlindEyePlayerCharacter::GetIsHunterAlwaysVisible()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return false;
	
	AGameStateBase* GameState = UGameplayStatics::GetGameState(World);
	if (GameState)
	{
		if (ABlindEyeGameState* BlindEyeGameState = Cast<ABlindEyeGameState>(GameState))
		{
			return BlindEyeGameState->bHunterAlwaysVisible;
		}
	}
	return false;
}

void ABlindEyePlayerCharacter::HealthUpdated()
{
	if (IsLocallyControlled())
	{
		// Update owning health UI
		UpdatePlayerHealthUI();
	} else
	{
		// Update health value of ally on owning player's UI
		ABlindEyePlayerState* BlindEyePlayerState = GetAllyPlayerState();
		ABlindEyePlayerCharacter* AllyBlindCharacter = Cast<ABlindEyePlayerCharacter>(BlindEyePlayerState->GetPawn());
		if (AllyBlindCharacter == nullptr) return;
		AllyBlindCharacter->UpdateAllyHealthUI();
	}
}

void ABlindEyePlayerCharacter::BirdMeterUpdated()
{
	if (IsLocallyControlled())
	{
		// TODO: Remove if not using this anymore
		// Update owning bird meter
		//UpdateBirdMeterUI();
	}
}

float ABlindEyePlayerCharacter::GetBirdMeterPercent()
{
	if (ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(GetPlayerState()))
	{
		return BlindEyePS->GetBirdMeter() / BlindEyePS->GetMaxBirdMeter();
	}
	return 0; 
}

float ABlindEyePlayerCharacter::GetBirdMeter()
{
	if (ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(GetPlayerState()))
	{
		return BlindEyePS->GetBirdMeter();
	}
	return 0;
}

void ABlindEyePlayerCharacter::OnGameLost()
{
	OnGameLostUI();
}

void ABlindEyePlayerCharacter::OnGameWon()
{
	OnGameWonUI();
}

float ABlindEyePlayerCharacter::GetHealthPercent()
{
	if (ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(GetPlayerState()))
	{
		return BlindEyePS->GetHealth() / BlindEyePS->GetMaxHealth();
	}
	return 0;
}

bool ABlindEyePlayerCharacter::GetIsDead()
{
	if (ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(GetPlayerState()))
	{
		return BlindEyePS->GetIsDead();
	}
	return false;
}

bool ABlindEyePlayerCharacter::IsActionsBlocked()
{
	ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(GetPlayerState());
	return BlindEyePS == nullptr || BlindEyePS->GetIsDead();
}

float ABlindEyePlayerCharacter::GetAllyHealthPercent()
{
	ABlindEyePlayerState* AllyState = GetAllyPlayerState();
	if (AllyState)
	{
		ABlindEyePlayerState* BlindAllyState = Cast<ABlindEyePlayerState>(AllyState);
		if (BlindAllyState == nullptr) return 0;
		return BlindAllyState->GetHealth() / BlindAllyState->GetMaxHealth();
	}
	return 0;
}

float ABlindEyePlayerCharacter::GetShrineHealthPercent()
{
	ABlindEyeGameState* BlindGameState = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(GetWorld()));
	AShrine* Shrine = BlindGameState->GetShrine();
	if (Shrine == nullptr) return 0;
	if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(Shrine))
	{
		return HealthInterface->GetHealthPercent();
	}
	return 0;
}

void ABlindEyePlayerCharacter::TryJump()
{
	TryFinishTutorial(ETutorialChecklist::Jump);
	if (!HealthComponent->GetIsHunterDebuff() && !GetIsDead())
	{
		Jump();
	}
}

void ABlindEyePlayerCharacter::MoveForward(float Value)
{
	if (IsActionsBlocked()) return;
	
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		if (AbilityManager->IsMovementBlocked()) return;
		float MovementAlter = AbilityManager->IsMovementSlowBlocked();

		// slow movement if hunter debuff active
		if (HealthComponent->GetIsHunterDebuff())
		{
			MovementAlter *= HunterMarkMovementAlter;
		}
		
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value * MovementAlter);
	}
}

void ABlindEyePlayerCharacter::MoveRight(float Value)
{
	if (IsActionsBlocked()) return;
	
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		if (AbilityManager->IsMovementBlocked()) return;
		float MovementAlter = AbilityManager->IsMovementSlowBlocked();

		// slow movement if hunter debuff active
		if (HealthComponent->GetIsHunterDebuff())
		{
			MovementAlter *= HunterMarkMovementAlter;
		}
		
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value * MovementAlter);
	}
}

void ABlindEyePlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABlindEyePlayerCharacter, bUnlimitedBirdMeter);
	DOREPLIFETIME(ABlindEyePlayerCharacter, PlayerType);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABlindEyePlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABlindEyePlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABlindEyePlayerCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABlindEyePlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABlindEyePlayerCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABlindEyePlayerCharacter::TryJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("BasicAttack", IE_Pressed, this, &ABlindEyePlayerCharacter::BasicAttackPressed);

	PlayerInputComponent->BindAction("ChargeBasicAttack", IE_Pressed, this, &ABlindEyePlayerCharacter::ChargedAttackPressed);
	PlayerInputComponent->BindAction("ChargeBasicAttack", IE_Released, this, &ABlindEyePlayerCharacter::ChargedAttackReleased);
	
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ABlindEyePlayerCharacter::DashPressed);
	PlayerInputComponent->BindAction("Dash", IE_Released, this, &ABlindEyePlayerCharacter::DashReleased);
	
	PlayerInputComponent->BindAction("Unique1", IE_Pressed, this, &ABlindEyePlayerCharacter::Unique1Pressed);
	PlayerInputComponent->BindAction("Unique1", IE_Released, this, &ABlindEyePlayerCharacter::Unique1Released);

	PlayerInputComponent->BindAction("Unique2", IE_Pressed, this, &ABlindEyePlayerCharacter::Unique2Pressed);
	PlayerInputComponent->BindAction("Unique2", IE_Released, this, &ABlindEyePlayerCharacter::Unique2Released);

	PlayerInputComponent->BindAction("Debug1", IE_Released, this, &ABlindEyePlayerCharacter::SER_DamageSelf);
	PlayerInputComponent->BindAction("Debug2", IE_Released, this, &ABlindEyePlayerCharacter::SER_DamageShrine);
}


