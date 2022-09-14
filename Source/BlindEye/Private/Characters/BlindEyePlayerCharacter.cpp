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
#include "Enemies/BurrowerEnemy.h"
#include "Enemies/SnapperEnemy.h"
#include "GameFramework/PlayerStart.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Gameplay/BlindEyePlayerState.h"
#include "Kismet/GameplayStatics.h"

//////////////////////////////////////////////////////////////////////////
// ATP_ThirdPersonCharacter

ABlindEyePlayerCharacter::ABlindEyePlayerCharacter()
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
	
	PlayerType = PlayerType::CrowPlayer;
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

void ABlindEyePlayerCharacter::Unique1Pressed()
{
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
	if (IsActionsBlocked()) return;
	AbilityManager->SER_UsedAbility(EAbilityTypes::Unique2, EAbilityInputTypes::Pressed);
}

void ABlindEyePlayerCharacter::Unique2Released()
{
	if (IsActionsBlocked()) return;
	AbilityManager->SER_UsedAbility(EAbilityTypes::Unique2, EAbilityInputTypes::Released);
}

void ABlindEyePlayerCharacter::SER_DebugInvincibility_Implementation()
{
	HealthComponent->IsInvincible = !HealthComponent->IsInvincible;
	if (GetLocalRole() == ROLE_Authority)
	{
		HealthComponent->OnRep_IsInvincibility();
	}
}

void ABlindEyePlayerCharacter::SER_DebugKillAllSnappers_Implementation()
{
	TArray<AActor*> SnapperActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASnapperEnemy::StaticClass(), SnapperActors);
	for (AActor* SnapperActor : SnapperActors)
	{
		if (const IHealthInterface* HealthInterface = Cast<IHealthInterface>(SnapperActor))
		{
			HealthInterface->Execute_GetHealthComponent(SnapperActor)->Kill();
		}
	}
}

void ABlindEyePlayerCharacter::SER_DebugKillAllBurrowers_Implementation()
{
	TArray<AActor*> BurrowerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABurrowerEnemy::StaticClass(), BurrowerActors);
	for (AActor* BurrowerActor : BurrowerActors)
	{
		if (const IHealthInterface* HealthInterface = Cast<IHealthInterface>(BurrowerActor))
		{
			HealthInterface->Execute_GetHealthComponent(BurrowerActor)->Kill();
		}
	}
}

void ABlindEyePlayerCharacter::SER_DebugKillAllHunters_Implementation()
{
	// TODO:
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

void ABlindEyePlayerCharacter::SER_ShrineInvincibility_Implementation()
{
	if (ABlindEyeGameState* BlindEyeGameState = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(GetWorld())))
	{
		AShrine* Shrine = BlindEyeGameState->GetShrine();
		if (Shrine != nullptr)
		{
			if (const IHealthInterface* ShrineHealthInterface = Cast<IHealthInterface>(Shrine))
			{
				UHealthComponent* ShrineHealthComp = ShrineHealthInterface->Execute_GetHealthComponent(Shrine);
				if (ShrineHealthComp == nullptr) return;
				ShrineHealthComp->IsInvincible = !ShrineHealthComp->IsInvincible;
				if (GetLocalRole() == ROLE_Authority)
				{
					ShrineHealthComp->OnRep_IsInvincibility();
				}
			}
		}
	}
}

float ABlindEyePlayerCharacter::GetHealth_Implementation()
{
	if (ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(GetPlayerState()))
		return BlindEyePS->GetHealth();
	return 0;
}

void ABlindEyePlayerCharacter::SetHealth_Implementation(float NewHealth)
{
	if (ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(GetPlayerState()))
		return BlindEyePS->SetHealth(NewHealth);
}

void ABlindEyePlayerCharacter::OnTakeDamage_Implementation(float Damage, FVector HitLocation, const UDamageType* DamageType,
	AActor* DamageCauser)
{}

void ABlindEyePlayerCharacter::OnDeath_Implementation() 
{
	if (ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(GetPlayerState()))
	{
		BlindEyePS->SetIsDead(true);
	}

	GetWorldTimerManager().SetTimer(AllyHealingCheckTimerHandle, this, &ABlindEyePlayerCharacter::SER_OnCheckAllyHealing, AllyHealCheckDelay, true);
}

bool ABlindEyePlayerCharacter::TryConsumeBirdMeter_Implementation(float PercentAmount)
{
	ABlindEyePlayerState* BlindEyePlayerState = Cast<ABlindEyePlayerState>(GetPlayerState());
	if (BlindEyePlayerState == nullptr) return false;

	float GetExactAmount = BlindEyePlayerState->GetMaxBirdMeter() * (PercentAmount / 100);
	float RemainingMeter = BlindEyePlayerState->GetBirdMeter();
	if (RemainingMeter >= GetExactAmount)
	{
		BlindEyePlayerState->SetBirdMeter(RemainingMeter - GetExactAmount);
		return true;
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
		// Update owning bird meter
		UpdateBirdMeterUI();
	}
}

float ABlindEyePlayerCharacter::GetBirdMeterPercent_Implementation()
{
	if (ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(GetPlayerState()))
	{
		return BlindEyePS->GetBirdMeter() / BlindEyePS->GetMaxBirdMeter();
	}
	return 0;
}

float ABlindEyePlayerCharacter::GetBirdMeter_Implementation()
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

float ABlindEyePlayerCharacter::GetHealthPercent_Implementation()
{
	if (ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(GetPlayerState()))
	{
		return BlindEyePS->GetHealth() / BlindEyePS->GetMaxHealth();
	}
	return 0;
}

bool ABlindEyePlayerCharacter::GetIsDead_Implementation()
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
	if (const IHealthInterface* HealthInterface = Cast<IHealthInterface>(Shrine))
	{
		return HealthInterface->Execute_GetHealthPercent(Shrine);
	}
	return 0;
}

void ABlindEyePlayerCharacter::MoveForward(float Value)
{
	if (IsActionsBlocked()) return;
	
	if (AbilityManager->IsMovementBlocked()) return;
	
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABlindEyePlayerCharacter::MoveRight(float Value)
{
	if (IsActionsBlocked()) return;
	
	if (AbilityManager->IsMovementBlocked()) return;
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
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

	PlayerInputComponent->BindAction("BasicAttack", IE_Pressed, this, &ABlindEyePlayerCharacter::BasicAttackPressed);

	PlayerInputComponent->BindAction("ChargeBasicAttack", IE_Pressed, this, &ABlindEyePlayerCharacter::ChargedAttackPressed);
	PlayerInputComponent->BindAction("ChargeBasicAttack", IE_Released, this, &ABlindEyePlayerCharacter::ChargedAttackReleased);
	
	PlayerInputComponent->BindAction("Unique1", IE_Pressed, this, &ABlindEyePlayerCharacter::Unique1Pressed);
	PlayerInputComponent->BindAction("Unique1", IE_Released, this, &ABlindEyePlayerCharacter::Unique1Released);

	PlayerInputComponent->BindAction("Unique2", IE_Pressed, this, &ABlindEyePlayerCharacter::Unique2Pressed);
	PlayerInputComponent->BindAction("Unique2", IE_Released, this, &ABlindEyePlayerCharacter::Unique2Released);

	PlayerInputComponent->BindAction("Debug1", IE_Released, this, &ABlindEyePlayerCharacter::SER_DebugInvincibility);
	PlayerInputComponent->BindAction("Debug2", IE_Released, this, &ABlindEyePlayerCharacter::SER_DebugKillAllSnappers);
	PlayerInputComponent->BindAction("Debug3", IE_Released, this, &ABlindEyePlayerCharacter::SER_DebugKillAllBurrowers);
	PlayerInputComponent->BindAction("Debug4", IE_Released, this, &ABlindEyePlayerCharacter::SER_DebugKillAllHunters);
	PlayerInputComponent->BindAction("Debug5", IE_Released, this, &ABlindEyePlayerCharacter::SER_DamageSelf);
	PlayerInputComponent->BindAction("Debug6", IE_Released, this, &ABlindEyePlayerCharacter::SER_DamageShrine);
	PlayerInputComponent->BindAction("Debug7", IE_Released, this, &ABlindEyePlayerCharacter::SER_ShrineInvincibility);
}


