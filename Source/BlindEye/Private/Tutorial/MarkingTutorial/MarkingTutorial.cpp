// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/MarkingTutorial/MarkingTutorial.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Tutorial/DummySpawnPoint.h"
#include "Tutorial/MarkingTutorial/MarkingDummyEnemy.h"

void AMarkingTutorial::BeginPlay()
{
	Super::BeginPlay();

	FString PhoenixText = TEXT("Use Marking ability with <Q> to mark enemies with your phoenix mark");
	AddChecklistItem(EPlayerType::PhoenixPlayer, 0, PhoenixText, NumTimesForEachPlayerToMark);
	 
	FString CrowText = TEXT("Use Marking ability with <Q> to mark enemies with your crow mark");
	AddChecklistItem(EPlayerType::CrowPlayer, 0, CrowText, NumTimesForEachPlayerToMark);
}

void AMarkingTutorial::SetupTutorial()
{
	Super::SetupTutorial();

	UWorld* World = GetWorld();
	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(World, SpawnPointTypes, SpawnPoints);
 
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
}

void AMarkingTutorial::EndTutorialLogic()
{
	Super::EndTutorialLogic();
}

void AMarkingTutorial::EndTutorial()
{
	Super::EndTutorial();

	UWorld* World = GetWorld();
	TArray<AActor*> Dummies;
	UGameplayStatics::GetAllActorsOfClass(World, ADummyEnemy::StaticClass(), Dummies);
	for (AActor* Dummy : Dummies)
	{
		if (ADummyEnemy* DummyEnemy = Cast<ADummyEnemy>(Dummy))
		{
			DummyEnemy->KillDummy();
		} else
		{
			Dummy->Destroy();
		}
	}
}

void AMarkingTutorial::PlayerEnteredTutorialHelper(ABlindEyePlayerCharacter* Player)
{
	Player->TutorialActionBlockers.bUnique2blocked = true;
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
		UpdateChecklistOfPlayer(EPlayerType::CrowPlayer, 0);
	}
	// Phoenix marked dummy
	else if (MarkerType == EMarkerType::Phoenix)
	{
		NumDummiesForPhoenixToMark--;
		if (NumDummiesForPhoenixToMark <= 0)
		{
			SetPlayerFinishedTutorial(EPlayerType::PhoenixPlayer);
		}
		UpdateChecklistOfPlayer(EPlayerType::PhoenixPlayer, 0);
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
