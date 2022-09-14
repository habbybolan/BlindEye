// Copyright (C) Nicholas Johnson 2022


#include "Characters/BlindEyePlayerController.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Gameplay/BlindEyeGameMode.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"

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
	PlayerType playerType;

	// TODO: Temporary for test spawning
	if (IsServer)
	{
		PlayerClassType = BlindEyeGameMode->PlayerClassTypes[0];
		playerType = PlayerType::CrowPlayer;
		IsServer = !IsServer;
	} else
	{
		PlayerClassType = BlindEyeGameMode->PlayerClassTypes[1];
		playerType = PlayerType::PhoenixPlayer;
		IsServer = !IsServer;
	}
		
	OwningCharacter = Cast<ABlindEyePlayerCharacter>(world->SpawnActor<ABlindEyePlayerCharacter>(PlayerClassType, spawnTransform, spawnParams));
	if (OwningCharacter)
	{
		OwningCharacter->PlayerType = playerType;
		APawn* CachedPawn = GetPawn();
		Possess(OwningCharacter);

		if (CachedPawn != nullptr)
		{
			CachedPawn->Destroy();
		}
	}
}

void ABlindEyePlayerController::CLI_GameLost_Implementation()
{
	if (GetPawn() == nullptr) return;
	if (ABlindEyePlayerCharacter* BlindEyeCharacter = Cast<ABlindEyePlayerCharacter>(GetPawn()))
	{
		BlindEyeCharacter->OnGameLost();
	}
}

void ABlindEyePlayerController::SER_RestartLevel_Implementation()
{
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());
	if (GameMode)
	{
		if (ABlindEyeGameMode* BlindEyeGameMode = Cast<ABlindEyeGameMode>(GameMode))
		{
			BlindEyeGameMode->RestartGame();
		}
	}
}

void ABlindEyePlayerController::CLI_GameWon_Implementation()
{
	if (GetPawn() == nullptr) return;
	if (ABlindEyePlayerCharacter* BlindEyeCharacter = Cast<ABlindEyePlayerCharacter>(GetPawn()))
	{
		BlindEyeCharacter->OnGameWon();
	}
}
