// Copyright (C) Nicholas Johnson 2022


#include "Gameplay/BlindEyeGameState.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

ABlindEyeGameState::ABlindEyeGameState()
{
}

void ABlindEyeGameState::BeginPlay()
{
	Super::BeginPlay();
	UWorld* world = GetWorld();
	if (world == nullptr) return;

	AActor* ShrineActor = UGameplayStatics::GetActorOfClass(world, AShrine::StaticClass());
	if (ShrineActor)
	{
		Shrine = MakeWeakObjectPtr(Cast<AShrine>(ShrineActor));
	}
 
	AActor* IslandManageActor = UGameplayStatics::GetActorOfClass(world, AIslandManager::StaticClass());
	if (IslandManageActor)
	{
		IslandManager = Cast<AIslandManager>(IslandManageActor);
	}
}

void ABlindEyeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABlindEyeGameState, bWinConditionPaused)
	DOREPLIFETIME(ABlindEyeGameState, bHunterAlwaysVisible)
	DOREPLIFETIME(ABlindEyeGameState, GameOverState)
}

ABlindEyePlayerCharacter* ABlindEyeGameState::GetRandomPlayer()
{
	return Cast<ABlindEyePlayerCharacter>(PlayerArray[UKismetMathLibrary::RandomIntegerInRange(0, PlayerArray.Num() - 1)]->GetPawn());
}

AShrine* ABlindEyeGameState::GetShrine()
{
	if (Shrine.IsValid())
		return Shrine.Get();
	return nullptr;
}

AIslandManager* ABlindEyeGameState::GetIslandManager()
{
	return IslandManager;
}

TArray<ABlindEyePlayerCharacter*> ABlindEyeGameState::GetPlayers()
{
	TArray<ABlindEyePlayerCharacter*> Players;
	for (APlayerState* PlayerState : PlayerArray)
	{
		ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PlayerState->GetPawn());
		check(Player)
		Players.Add(Player);
	}
	return Players;
}

ABlindEyePlayerCharacter* ABlindEyeGameState::GetPlayer(EPlayerType PlayerType)
{
	for (APlayerState* PlayerState : PlayerArray)
	{
		ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PlayerState->GetPawn());
		check(Player)
		if (Player->PlayerType == PlayerType) return Player;
	}
	// PlayerType player not connected
	return nullptr;
}
