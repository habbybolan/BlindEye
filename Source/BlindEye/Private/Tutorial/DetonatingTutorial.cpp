// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/DetonatingTutorial.h"

#include "Kismet/GameplayStatics.h"
#include "Tutorial/DummySpawnPoint.h"

void ADetonatingTutorial::SetupTutorial()
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
		ADetonatingDummyEnemy* Dummy = World->SpawnActor<ADetonatingDummyEnemy>(DummyType, DummySpawnPoint->GetActorLocation(), DummySpawnPoint->GetActorRotation(), Params);

		// Setup Dummy and subscribe to detonation events
		if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(Dummy))
		{
			HealthInterface->GetHealthComponent()->DetonateDelegate.AddDynamic(this, &ADetonatingTutorial::OnMarkDetonated);

			if (DummySpawnPoint->DummyType == EDummySpawnType::Crow)
			{
				NumPhoenixMarksToDetonate++;
				HealthInterface->GetHealthComponent()->AddMarkerHelper(EMarkerType::Phoenix);
			} else if (DummySpawnPoint->DummyType == EDummySpawnType::Phoenix)
			{
				NumCrowMarksToDetonate++;
				HealthInterface->GetHealthComponent()->AddMarkerHelper(EMarkerType::Crow);
			}
		}
	}
}

void ADetonatingTutorial::SetTutorialRunning()
{
	Super::SetTutorialRunning();
}

void ADetonatingTutorial::EndTutorial()
{
	Super::EndTutorial();
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Green, "TUTORIAL FINISHED");

	UWorld* World = GetWorld();
	TArray<AActor*> Dummies;
	UGameplayStatics::GetAllActorsOfClass(World, ADummyEnemy::StaticClass(), Dummies);
	for (AActor* Dummy : Dummies)
	{
		Dummy->Destroy();
	}
}

void ADetonatingTutorial::OnMarkDetonated(AActor* MarkedPawn, EMarkerType MarkerType)
{
	if (MarkerType == EMarkerType::Crow)
	{
		NumCrowMarksToDetonate--;
		// If all crow marks detonated, notify Phoenix player finished
		if (NumCrowMarksToDetonate <= 0)
		{
			SetPlayerFinishedTutorial(EPlayerType::PhoenixPlayer);
		}
	} else if (MarkerType == EMarkerType::Phoenix)
	{ 
		NumPhoenixMarksToDetonate--;
		// If all phoenix marks detonated, notify Phoenix player finished
		if (NumPhoenixMarksToDetonate <= 0)
		{
			SetPlayerFinishedTutorial(EPlayerType::CrowPlayer);
		}
	}
}

void ADetonatingTutorial::SetPlayerFinishedTutorial(EPlayerType PlayerThatFinished)
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
