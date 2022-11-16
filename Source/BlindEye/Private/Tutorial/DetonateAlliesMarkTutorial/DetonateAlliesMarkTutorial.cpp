// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/DetonateAlliesMarkTutorial/DetonateAlliesMarkTutorial.h"

#include "Kismet/GameplayStatics.h"
#include "Tutorial/DummySpawnPoint.h"

void ADetonateAlliesMarkTutorial::SetupTutorial()
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
		ADummyEnemy* Dummy = World->SpawnActor<ADummyEnemy>(DummyType, DummySpawnPoint->GetActorLocation(), DummySpawnPoint->GetActorRotation(), Params);

		// Setup Dummy and subscribe to detonation events
		if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(Dummy))
		{
			HealthInterface->GetHealthComponent()->DetonateDelegate.AddDynamic(this, &ADetonateAlliesMarkTutorial::OnMarkDetonated);
		}

		// Set tutorial end condition
		NumCrowMarksToDetonate = NumTimesForEachPlayerToDetonate;
		NumPhoenixMarksToDetonate = NumTimesForEachPlayerToDetonate;
	}

	FString PhoenixText = TEXT("Combine what you've learned to mark and detonate enemies with your ally");
	AddChecklistItem(EPlayerType::PhoenixPlayer, 0, PhoenixText, NumTimesForEachPlayerToDetonate);
	 
	FString CrowText = TEXT("Combine what you've learned to mark and detonate enemies with your ally");
	AddChecklistItem(EPlayerType::CrowPlayer, 0, CrowText, NumTimesForEachPlayerToDetonate);
}

void ADetonateAlliesMarkTutorial::EndTutorial()
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

void ADetonateAlliesMarkTutorial::OnMarkDetonated(AActor* MarkedPawn, EMarkerType MarkerType)
{
	if (MarkerType == EMarkerType::Crow)
	{
		NumCrowMarksToDetonate--;
		// If all crow marks detonated, notify Phoenix player finished
		if (NumCrowMarksToDetonate <= 0)
		{
			SetPlayerFinishedTutorial(EPlayerType::PhoenixPlayer);
		}
		UpdateChecklistOfPlayer(EPlayerType::PhoenixPlayer, 0);
	} else if (MarkerType == EMarkerType::Phoenix)
	{
		NumPhoenixMarksToDetonate--;
		// If all phoenix marks detonated, notify Phoenix player finished
		if (NumPhoenixMarksToDetonate <= 0)
		{
			SetPlayerFinishedTutorial(EPlayerType::CrowPlayer);
		}
		UpdateChecklistOfPlayer(EPlayerType::CrowPlayer, 0);
	}
}

void ADetonateAlliesMarkTutorial::SetPlayerFinishedTutorial(EPlayerType PlayerThatFinished)
{
	// Set player as finished, if both players finished, end this tutorial
	if (PlayerThatFinished == EPlayerType::CrowPlayer)
	{
		bPhoenixPlayerFinished = true;
		if (bCrowPlayerFinished)
		{
			EndTutorial();
		}
	} else
	{
		bCrowPlayerFinished = true;
		if (bPhoenixPlayerFinished)
		{
			EndTutorial();
		}
	}
}
