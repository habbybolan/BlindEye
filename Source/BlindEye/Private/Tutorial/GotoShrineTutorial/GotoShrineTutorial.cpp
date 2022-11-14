// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/GotoShrineTutorial/GotoShrineTutorial.h"

#include "BlindEyeBaseCharacter.h"
#include "Shrine.h"
#include "Gameplay/BlindEyeGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

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
