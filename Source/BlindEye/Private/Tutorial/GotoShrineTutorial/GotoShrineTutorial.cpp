// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/GotoShrineTutorial/GotoShrineTutorial.h"

#include "BlindEyeBaseCharacter.h"
#include "Shrine.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/BlindEyeGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void AGotoShrineTutorial::BeginPlay()
{
	Super::BeginPlay();

	FString PhoenixText = TEXT("When you're ready, go to the shrine");
	AddChecklistItem(EPlayerType::PhoenixPlayer, 0, PhoenixText, 0);
	 
	FString CrowText = TEXT("When you're ready, go to the shrine");
	AddChecklistItem(EPlayerType::CrowPlayer, 0, CrowText, 0);
}

void AGotoShrineTutorial::SetupTutorial()
{
	Super::SetupTutorial();
	StartWaitingForPlayersToInteract();
}

void AGotoShrineTutorial::EndTutorial()
{
	Super::EndTutorial();
 
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CheckPlayersNearTimerHandle);
	}

	check(BlindEyeGS)
	for (APlayerState* PS : BlindEyeGS->PlayerArray)
	{
		if (PS->GetPawn())
		{
			ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PS->GetPawn());
			Player->RemoveScreenIndicator(GotoShrineIndicatorID);
		}
	}
}

void AGotoShrineTutorial::OnPlayerConnected(ABlindEyePlayerCharacter* Player)
{
	Player->AddScreenIndicator(GotoShrineIndicatorID, GotoShrineIndicator, BlindEyeGS->GetShrine(), 0);
}

void AGotoShrineTutorial::StartWaitingForPlayersToInteract()
{
	UWorld* World = GetWorld();
	if (World)
	{
		ShrineActor = UGameplayStatics::GetActorOfClass(World, AShrine::StaticClass());
		check(ShrineActor)
		World->GetTimerManager().SetTimer(CheckPlayersNearTimerHandle, this, &AGotoShrineTutorial::CheckPlayersNearbyHelper, 0.5, true);
	}
}

void AGotoShrineTutorial::CheckPlayersNearbyHelper()
{
	TArray<AActor*> OutActors;
	UWorld* World = GetWorld();
	UKismetSystemLibrary::SphereOverlapActors(World, ShrineActor->GetActorLocation(), CheckTutorialPlayersRadius, TArray<TEnumAsByte<EObjectTypeQuery>>(),
		ABlindEyeBaseCharacter::StaticClass(), TArray<AActor*>(), OutActors);

	// If 2 players near shrine, then end beginning tutorial and start first enemy tutorial
	if (OutActors.Num() >= BlindEyeGS->PlayerArray.Num())
	{
		// End Tutorial
		EndTutorial();
	}
}
