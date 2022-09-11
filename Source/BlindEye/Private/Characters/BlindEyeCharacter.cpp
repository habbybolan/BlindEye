// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/BlindEyeCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Abilities/AbilityManager.h"
#include "Characters/BlindEyePlayerController.h"
#include "Components/HealthComponent.h"
#include "Enemies/BlindEyeEnemyController.h"
#include "Enemies/BurrowerEnemy.h"
#include "Enemies/SnapperEnemy.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Gameplay/BlindEyePlayerState.h"
#include "Kismet/GameplayStatics.h"

//////////////////////////////////////////////////////////////////////////
// ATP_ThirdPersonCharacter

ABlindEyeCharacter::ABlindEyeCharacter()
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

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	
	PlayerType = PlayerType::CrowPlayer;
	Team = TEAMS::Player;
} 

void ABlindEyeCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled())
	{
		UWorld* world = GetWorld();
		if (world == nullptr) return;

		AActor* ShrineActor = UGameplayStatics::GetActorOfClass(world, AShrine::StaticClass());
		if (ShrineActor)
		{
			AShrine* Shrine = Cast<AShrine>(ShrineActor);
			Shrine->ShrineHealthChange.AddUFunction(this, TEXT("UpdateShrineHealthUI"));
		}
	}
}


ABlindEyePlayerState* ABlindEyeCharacter::GetAllyPlayerState()
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

void ABlindEyeCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (IsLocallyControlled())
	{
		UpdateAllClientUI();
	}
}

void ABlindEyeCharacter::UpdateAllClientUI()
{
	UpdatePlayerHealthUI();
}

void ABlindEyeCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ABlindEyeCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABlindEyeCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ABlindEyeCharacter::BasicAttackPressed() 
{
	AbilityManager->SER_UsedAbility(EAbilityTypes::Basic, EAbilityInputTypes::Pressed);
}

void ABlindEyeCharacter::ChargedAttackPressed()
{
	AbilityManager->SER_UsedAbility(EAbilityTypes::ChargedBasic, EAbilityInputTypes::Pressed);
}

void ABlindEyeCharacter::ChargedAttackReleased()
{
	AbilityManager->SER_UsedAbility(EAbilityTypes::ChargedBasic, EAbilityInputTypes::Released);
}

void ABlindEyeCharacter::Unique1Pressed()
{
	AbilityManager->SER_UsedAbility(EAbilityTypes::Unique1, EAbilityInputTypes::Pressed);
}

void ABlindEyeCharacter::Unique1Released()
{
	AbilityManager->SER_UsedAbility(EAbilityTypes::Unique1, EAbilityInputTypes::Released);
}

void ABlindEyeCharacter::Unique2Pressed()
{
	AbilityManager->SER_UsedAbility(EAbilityTypes::Unique2, EAbilityInputTypes::Pressed);
}

void ABlindEyeCharacter::Unique2Released()
{
	AbilityManager->SER_UsedAbility(EAbilityTypes::Unique2, EAbilityInputTypes::Released);
}

void ABlindEyeCharacter::SER_DebugInvincibility_Implementation()
{
	HealthComponent->IsInvincible = !HealthComponent->IsInvincible;
	if (GetLocalRole() == ROLE_Authority)
	{
		HealthComponent->OnRep_IsInvincibility();
	}
}

void ABlindEyeCharacter::SER_DebugKillAllSnappers_Implementation()
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

void ABlindEyeCharacter::SER_DebugKillAllBurrowers_Implementation()
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

void ABlindEyeCharacter::SER_DebugKillAllHunters_Implementation()
{
	// TODO:
}

float ABlindEyeCharacter::GetHealth_Implementation()
{
	if (ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(GetPlayerState()))
		return BlindEyePS->GetHealth();
	return 0;
}

void ABlindEyeCharacter::SetHealth_Implementation(float NewHealth)
{
	if (ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(GetPlayerState()))
		return BlindEyePS->SetHealth(NewHealth);
}

void ABlindEyeCharacter::OnTakeDamage_Implementation(float Damage, FVector HitLocation, const UDamageType* DamageType,
	AActor* DamageCauser)
{}

UHealthComponent* ABlindEyeCharacter::GetHealthComponent_Implementation()
{
	return HealthComponent;
}

void ABlindEyeCharacter::OnDeath_Implementation()
{
	// TODO:
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, "Player Died");
}

TEAMS ABlindEyeCharacter::GetTeam_Implementation()
{
	return Team;
}

void ABlindEyeCharacter::HealthUpdated()
{
	if (IsLocallyControlled())
	{
		// Update owning health UI
		UpdatePlayerHealthUI();
	} else
	{
		// Update health on owning characters UI
		ABlindEyePlayerState* BlindEyePlayerState = GetAllyPlayerState();
		ABlindEyeCharacter* AllyBlindCharacter = Cast<ABlindEyeCharacter>(BlindEyePlayerState->GetPawn());
		if (AllyBlindCharacter == nullptr) return;
		AllyBlindCharacter->UpdateAllyHealthUI();
	}
}
 
float ABlindEyeCharacter::GetHealthPercent_Implementation()
{
	if (ABlindEyePlayerState* BlindEyePS = Cast<ABlindEyePlayerState>(GetPlayerState()))
	{
		return BlindEyePS->GetHealth() / BlindEyePS->GetMaxHealth();
	}
	return 0;
}

float ABlindEyeCharacter::GetAllyHealthPercent()
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

float ABlindEyeCharacter::GetShrineHealthPercent()
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

void ABlindEyeCharacter::MoveForward(float Value)
{
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

void ABlindEyeCharacter::MoveRight(float Value)
{
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

void ABlindEyeCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABlindEyeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABlindEyeCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABlindEyeCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABlindEyeCharacter::LookUpAtRate);

	// TODO: Player input for Basic attack
	PlayerInputComponent->BindAction("BasicAttack", IE_Pressed, this, &ABlindEyeCharacter::BasicAttackPressed);

	PlayerInputComponent->BindAction("ChargeBasicAttack", IE_Pressed, this, &ABlindEyeCharacter::ChargedAttackPressed);
	PlayerInputComponent->BindAction("ChargeBasicAttack", IE_Released, this, &ABlindEyeCharacter::ChargedAttackReleased);
	
	PlayerInputComponent->BindAction("Unique1", IE_Pressed, this, &ABlindEyeCharacter::Unique1Pressed);
	PlayerInputComponent->BindAction("Unique1", IE_Released, this, &ABlindEyeCharacter::Unique1Released);

	PlayerInputComponent->BindAction("Unique2", IE_Pressed, this, &ABlindEyeCharacter::Unique2Pressed);
	PlayerInputComponent->BindAction("Unique2", IE_Released, this, &ABlindEyeCharacter::Unique2Released);
	// TODO: Player input for rest of attacks

	PlayerInputComponent->BindAction("Debug1", IE_Released, this, &ABlindEyeCharacter::SER_DebugInvincibility);
	PlayerInputComponent->BindAction("Debug2", IE_Released, this, &ABlindEyeCharacter::SER_DebugKillAllSnappers);
	PlayerInputComponent->BindAction("Debug3", IE_Released, this, &ABlindEyeCharacter::SER_DebugKillAllBurrowers);
}


