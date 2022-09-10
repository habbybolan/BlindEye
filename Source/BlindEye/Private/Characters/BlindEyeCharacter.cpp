// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/BlindEyeCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Abilities/AbilityManager.h"
#include "Components/HealthComponent.h"
#include "Gameplay/BlindEyePlayerState.h"

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
	if (GetLocalRole() == ROLE_Authority)
	{
		SetupPlayerState();
	}
}

void ABlindEyeCharacter::SetupPlayerState()
{
	BlindEyePlayerState = Cast<ABlindEyePlayerState>(GetPlayerState());
	if (!BlindEyePlayerState) return;

	BlindEyePlayerState->HealthUpdated.AddUFunction(this, TEXT("HealthUpdated"));
}

void ABlindEyeCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	SetupPlayerState();
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
	// Note: Only called from server
	BlindEyePlayerState = Cast<ABlindEyePlayerState>(GetPlayerState());
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

void ABlindEyeCharacter::Unique1Pressed()
{
	AbilityManager->SER_UsedAbility(EAbilityTypes::Unique1, EAbilityInputTypes::Pressed);
}

void ABlindEyeCharacter::Unique1Released()
{
	AbilityManager->SER_UsedAbility(EAbilityTypes::Unique1, EAbilityInputTypes::Released);
}

float ABlindEyeCharacter::GetHealth_Implementation()
{
	if (BlindEyePlayerState)
		return BlindEyePlayerState->GetHealth();
	return 0;
}

void ABlindEyeCharacter::SetHealth_Implementation(float NewHealth)
{
	if (BlindEyePlayerState)
		return BlindEyePlayerState->SetHealth(NewHealth);
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
		UpdatePlayerHealthUI();
	} 
}

float ABlindEyeCharacter::GetHealthPercent()
{
	if (BlindEyePlayerState == nullptr) return 0;
	return BlindEyePlayerState->GetHealth() / BlindEyePlayerState->GetMaxHealth();
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
	
	PlayerInputComponent->BindAction("Unique1", IE_Pressed, this, &ABlindEyeCharacter::Unique1Pressed);
	PlayerInputComponent->BindAction("Unique1", IE_Released, this, &ABlindEyeCharacter::Unique1Released);
	// TODO: Player input for rest of attacks
}


