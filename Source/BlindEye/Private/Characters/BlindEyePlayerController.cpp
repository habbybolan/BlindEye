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
		
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(GetWorld()));
		BlindEyeGS->BlindEyePlayers.Add(OwningCharacter);
		if (BlindEyeGS->BlindEyePlayers.Num() >= 2)
		{
			BlindEyeGS->BlindEyePlayers[0]->NotifyOfOtherPlayerExistance(BlindEyeGS->BlindEyePlayers[1]);
			BlindEyeGS->BlindEyePlayers[1]->NotifyOfOtherPlayerExistance(BlindEyeGS->BlindEyePlayers[0]);
		}
	}
}

void ABlindEyePlayerController::CLI_GameLost_Implementation()
{
	if (GetPawn() == nullptr) return;
	if (ABlindEyePlayerCharacter* BlindEyeCharacter = Cast<ABlindEyePlayerCharacter>(GetPawn()))
	{
		BlindEyeCharacter->CLI_OnGameLost();
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
		BlindEyeCharacter->CLI_OnGameWon();
	}
}

void ABlindEyePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsLocalController())
	{
		
	}
}
