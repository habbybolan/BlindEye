// Copyright (C) Nicholas Johnson 2022


#include "CharacterSelect/CharacterSelectPawn.h"

#include "Camera/CameraComponent.h"
#include "CharacterSelect/CharacterSelectGameState.h"
#include "CharacterSelect/CharacterSelectModel.h"
#include "CharacterSelect/CharacterSelectPlayerController.h"
#include "CharacterSelect/CharacterSelectPlayerState.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"

ACharacterSelectPawn::ACharacterSelectPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("Camera");
	FollowCamera->SetupAttachment(RootComponent);
}

void ACharacterSelectPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ACharacterSelectPawn::PlayerTrySelectCharacter()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	// Linecast from screen to check if player trying to select one of the characters
	if (GetController()) 
	{
		ACharacterSelectPlayerController* PlayerController = Cast<ACharacterSelectPlayerController>(GetController());
		FVector ClickLocation;
		FVector ClickRotation;
		PlayerController->DeprojectMousePositionToWorld(ClickLocation, ClickRotation);

		TArray<FHitResult> OutModels;
		if (UKismetSystemLibrary::LineTraceMultiForObjects(World, ClickLocation, ClickLocation + ClickRotation * 1000,
			CharacterModelObjectType, false, TArray<AActor*>(), EDrawDebugTrace::None, OutModels, true))
		{
			// Set character as selected
			ACharacterSelectModel* CSModel = Cast<ACharacterSelectModel>(OutModels[0].Actor);
			SER_PlayerSelectHelper(CSModel->PlayerType);
			
		}
	}	
}

void ACharacterSelectPawn::SER_PlayerSelectHelper_Implementation(EPlayerType CharacterSelected)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	AGameStateBase* GameStateBase = UGameplayStatics::GetGameState(World);
	if (GameStateBase == nullptr) return;
	ACharacterSelectGameState* CharacterSelectGS = Cast<ACharacterSelectGameState>(GameStateBase);

	if (GetController())
	{
		ACharacterSelectPlayerController* PlayerController = Cast<ACharacterSelectPlayerController>(GetController());
		CharacterSelectGS->PlayerTrySelect(CharacterSelected, PlayerController);
	}
}

void ACharacterSelectPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &ACharacterSelectPawn::PlayerTrySelectCharacter);
}

