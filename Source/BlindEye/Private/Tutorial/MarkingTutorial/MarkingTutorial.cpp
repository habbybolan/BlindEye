// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/MarkingTutorial/MarkingTutorial.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Tutorial/DummySpawnPoint.h"
#include "Tutorial/MarkingTutorial/MarkingDummyEnemy.h"

void AMarkingTutorial::SetupTutorial()
{
	Super::SetupTutorial();

	UWorld* World = GetWorld();
	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(World, ADummySpawnPoint::StaticClass(), SpawnPoints);
 
	for (AActor* SpawnActor : SpawnPoints)
	{
		ADummySpawnPoint* DummySpawnPoint = Cast<ADummySpawnPoint>(SpawnActor);

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AMarkingDummyEnemy* Dummy = World->SpawnActor<AMarkingDummyEnemy>(DummyType, DummySpawnPoint->GetActorLocation(), DummySpawnPoint->GetActorRotation(), Params);

		// Setup Dummy and subscribe to marking events
		if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(Dummy))
		{
			HealthInterface->GetHealthComponent()->MarkedAddedDelegate.AddDynamic(this, &AMarkingTutorial::OnDummyMarked);
		} 
	}

	// Set tutorial end condition
	NumDummiesForCrowToMark = NumTimesForEachPlayerToMark;
	NumDummiesForPhoenixToMark = NumTimesForEachPlayerToMark;

	// Set ability 2 as blocked
	for (APlayerState* PS : BlindEyeGS->PlayerArray)
	{
		if (PS->GetPawn())
		{
			ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PS->GetPawn());
			check(Player)
			Player->TutorialActionBlockers.bUnique2blocked = true;
		}
	}
}

void AMarkingTutorial::EndTutorial()
{
	Super::EndTutorial();

	UWorld* World = GetWorld();
	TArray<AActor*> Dummies;
	UGameplayStatics::GetAllActorsOfClass(World, ADummyEnemy::StaticClass(), Dummies);
	for (AActor* Dummy : Dummies)
	{
		Dummy->Destroy();
	}
}

void AMarkingTutorial::OnDummyMarked(AActor* MarkedPawn, EMarkerType MarkerType)
{
	// Crow marked dummy
	if (MarkerType == EMarkerType::Crow)
	{
		NumDummiesForCrowToMark--;
		if (NumDummiesForCrowToMark <= 0)
		{
			SetPlayerFinishedTutorial(EPlayerType::CrowPlayer);
		}
	}
	// Phoenix marked dummy
	else if (MarkerType == EMarkerType::Phoenix)
	{
		NumDummiesForPhoenixToMark--;
		if (NumDummiesForPhoenixToMark <= 0)
		{
			SetPlayerFinishedTutorial(EPlayerType::PhoenixPlayer);
		}
	}
}

void AMarkingTutorial::SetPlayerFinishedTutorial(EPlayerType PlayerThatFinished)
{
	// Set player as finished, if both players finished, end this tutorial
	if (PlayerThatFinished == EPlayerType::CrowPlayer)
	{
		bCrowPlayerFinished = true;
		if (bPhoenixPlayerFinished)
		{
			EndTutorial();
		}
	} else
	{
		bPhoenixPlayerFinished = true;
		if (bCrowPlayerFinished)
		{
			EndTutorial();
		}
	}
}
