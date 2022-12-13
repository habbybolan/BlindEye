// Copyright (C) Nicholas Johnson 2022


#include "Characters/BlindEyePlayerController.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Gameplay/BlindEyeGameMode.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"
#include "BlindEyeUtils.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/BlindEyeGameInstance.h"

class ABlindEyeGameState;

void ABlindEyePlayerController::SER_SpawnPlayer_Implementation()
{
	UWorld* world = GetWorld();
	if (!world) return;

	ABlindEyeGameMode* BlindEyeGameMode = Cast<ABlindEyeGameMode>(UGameplayStatics::GetGameMode(world));
	if (BlindEyeGameMode == nullptr) return;

	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FTransform spawnTransform = BlindEyeGameMode->GetSpawnPoint();
	TSubclassOf<ABlindEyePlayerCharacter> PlayerClassType;
	EPlayerType playerType;

	UBlindEyeGameInstance* BlindEyeGI = Cast<UBlindEyeGameInstance>(GetGameInstance());

	check(PlayerState)

	playerType = BlindEyeGI->GetPlayerType(PlayerState);
	if (playerType == EPlayerType::CrowPlayer)
	{
		PlayerClassType = BlindEyeGameMode->CrowClassType;
	} else
	{
		PlayerClassType = BlindEyeGameMode->PhoenixClassType;
	}

	// For stupid spawning when playing in editor
	if (BlindEyeGI->bInEditor)
	{
		if (IsServer)
		{
			PlayerClassType = BlindEyeGameMode->CrowClassType;
			playerType = EPlayerType::CrowPlayer;
			IsServer = !IsServer;
		} else
		{
			PlayerClassType = BlindEyeGameMode->PhoenixClassType;
			playerType = EPlayerType::PhoenixPlayer;
			IsServer = !IsServer;
		}
	}
		
	OwningCharacter = Cast<ABlindEyePlayerCharacter>(world->SpawnActorDeferred<ABlindEyePlayerCharacter>(PlayerClassType,
		spawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));
	OwningCharacter->PlayerType = playerType;
	OwningCharacter->FinishSpawning(spawnTransform);
	if (OwningCharacter)
	{
		APawn* CachedPawn = GetPawn();
		Possess(OwningCharacter);

		if (CachedPawn != nullptr)
		{
			CachedPawn->Destroy();
		}
	}
}

void ABlindEyePlayerController::SER_RestartLevel_Implementation()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	// Play loading screen for game restart
	if (AGameStateBase* GameState = UGameplayStatics::GetGameState(World))
	{
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(GameState);
		BlindEyeGS->MULT_StartLoadingScreen();
	}

	// restart the level
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(World);
	if (GameMode)
	{
		if (ABlindEyeGameMode* BlindEyeGameMode = Cast<ABlindEyeGameMode>(GameMode))
		{
			BlindEyeGameMode->RestartGame();
		}
	}
}

void ABlindEyePlayerController::SER_CharacterSelect_Implementation()
{
	if (UWorld* World = GetWorld())
	{
		// Play loading screen for both players
		if (AGameStateBase* GameState = UGameplayStatics::GetGameState(World))
		{
			ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(GameState);
			BlindEyeGS->MULT_StartLoadingScreen();
		}
	}
	
	UBlindEyeGameInstance* BlindEyeGI = Cast<UBlindEyeGameInstance>(GetGameInstance());
	BlindEyeGI->SER_LoadCharacterSelectMap();
}

void ABlindEyePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsLocalController())
	{
		
	}
}
