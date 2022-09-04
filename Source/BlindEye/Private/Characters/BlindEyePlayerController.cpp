// Copyright (C) Nicholas Johnson 2022


#include "Characters/BlindEyePlayerController.h"

#include "Characters/BlindEyeCharacter.h"
#include "Gameplay/BlindEyeGameMode.h"
#include "Kismet/GameplayStatics.h"

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
	TSubclassOf<ABlindEyeCharacter> PlayerClassType;

	// TODO: Temporary for test spawning
	if (IsServer)
	{
		PlayerClassType = BlindEyeGameMode->PlayerClassTypes[0];
		IsServer = !IsServer;
	} else
	{
		PlayerClassType = BlindEyeGameMode->PlayerClassTypes[1];
	}
		
	OwningCharacter = Cast<ABlindEyeCharacter>(world->SpawnActor<ABlindEyeCharacter>(PlayerClassType, spawnTransform, spawnParams));
	if (OwningCharacter)
	{
		APawn* CachedPawn = GetPawn();
		Possess(OwningCharacter);

		if (CachedPawn != nullptr)
		{
			CachedPawn->Destroy();
		}
	}
}
