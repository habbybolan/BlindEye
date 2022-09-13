// Copyright (C) Nicholas Johnson 2022


#include "Gameplay/BlindEyeGameMode.h"

#include "EngineUtils.h"
#include "Characters/BlindEyePlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Gameplay/BlindEyeGameState.h"

void ABlindEyeGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (ABlindEyePlayerController* BlindEyeController = Cast<ABlindEyePlayerController>(NewPlayer))
	{
		BlindEyeController->SER_SpawnPlayer();
		// TODO: Any match logic needed
	}
}

FTransform ABlindEyeGameMode::GetSpawnPoint() const
{
	for (TActorIterator<APlayerStart> playerStart(GetWorld()); playerStart; ++playerStart)
	{
		if (playerStart->PlayerStartTag != "Spawned")
		{
			playerStart->PlayerStartTag = "Spawned";
			return playerStart->GetTransform();
		}
	}
	return FTransform();
}

void ABlindEyeGameMode::OnShrineDeath()
{
	OnGameEnded();
}

void ABlindEyeGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();
	if (!world) return;

	world->SpawnActor(HunterControllerType);
}

void ABlindEyeGameMode::OnGameEnded()
{
	UWorld* World = GetWorld();
	if (!World) return;
	
	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (ABlindEyePlayerController* BlindEyePlayerController = Cast<ABlindEyePlayerController>(PlayerController))
		{
			BlindEyePlayerController->CLI_GameEnded();
		}
	}

	World->GetTimerManager().SetTimer(GameRestartTimerHandle, this, &ABlindEyeGameMode::RestartGame, 5, false);
}

void ABlindEyeGameMode::RestartGame()
{
	Super::RestartGame();
}
