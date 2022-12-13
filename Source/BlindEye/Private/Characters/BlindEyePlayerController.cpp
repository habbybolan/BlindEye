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
		
	OwningCharacter = Cast<ABlindEyePlayerCharacter>(world->SpawnActorDeferred<ABlindEyePlayerCharacter>(PlayerClassType,
		spawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));
	OwningCharacter->PlayerType = playerType;
	OwningCharacter->FinishSpawning(spawnTransform);
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

void ABlindEyePlayerController::SER_RestartLevel_Implementation()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	// Play loading screen for game restart
	if (AGameStateBase* GameState = UGameplayStatics::GetGameState(World))
	{
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(GameState);
		BlindEyeGS->MULT_StartLoadingScreen();
	}

	// restart the level
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(World);
	if (GameMode)
	{
		if (ABlindEyeGameMode* BlindEyeGameMode = Cast<ABlindEyeGameMode>(GameMode))
		{
			BlindEyeGameMode->RestartGame();
		}
	}
}

void ABlindEyePlayerController::SER_CharacterSelect_Implementation()
{
	if (UWorld* World = GetWorld())
	{
		// Play loading screen for both players
		if (AGameStateBase* GameState = UGameplayStatics::GetGameState(World))
		{
			ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(GameState);
			BlindEyeGS->MULT_StartLoadingScreen();
		}
	}
	
	UBlindEyeGameInstance* BlindEyeGI = Cast<UBlindEyeGameInstance>(GetGameInstance());
	BlindEyeGI->SER_LoadCharacterSelectMap();
}

void ABlindEyePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

FRotator ABlindEyePlayerController::GetDesiredRotationFromMouse()
{
	FVector MouseLocation;
	FVector MouseRotation;
	DeprojectMousePositionToWorld(OUT MouseLocation, OUT MouseRotation);
	
	if (ensure(GetPawn()))
	{
		if (UWorld* World = GetWorld())
		{
			FVector DirToMouse;
			FHitResult OutHit;
			if (UKismetSystemLibrary::LineTraceSingle(World, MouseLocation, MouseLocation + MouseRotation * 5000, ETraceTypeQuery::TraceTypeQuery1,
				false, TArray<AActor*>(), EDrawDebugTrace::None, OutHit, true))
			{
				DirToMouse = (OutHit.Location - GetPawn()->GetActorLocation()) * FVector(1, 1, 0);
			} else
			{
				DirToMouse = (MouseLocation + MouseRotation * 1000 - GetPawn()->GetActorLocation()) * FVector(1, 1, 0);
			}
			return DirToMouse.Rotation();
		}
	}
	return FRotator::ZeroRotator;
}

/**
 * Static helper for calculating the hit location of a mouse from its location and rotation
 * @param MouseLocation		Location of the mouse in world space
 * @param MouseRotation		Rotation of mouse in World Space
 * @param Character			Player Character's mouse
 * @param World				World
 * @param HitObjectType		Objects trace will hit. If left empty, then defaults will be WorldStatic and WorldDynamic
 */
FVector ABlindEyePlayerController::GetMouseAimLocationHelper(FVector MouseLocation, FRotator MouseRotation, ACharacter* Character,
		UWorld* World, TArray<TEnumAsByte<EObjectTypeQuery>> HitObjectType)
{
	// If no Object types added, set as defaults of WorldStatic and WorldDynamic
	if (HitObjectType.Num() == 0)
	{
		HitObjectType.Add(EObjectTypeQuery::ObjectTypeQuery1);
		HitObjectType.Add(EObjectTypeQuery::ObjectTypeQuery2);
	}
	
	FHitResult OutHit;
	// Set mouse aim location to cast from mouse
	if (UKismetSystemLibrary::LineTraceSingleForObjects(World, MouseLocation, MouseLocation + MouseRotation.Vector() * 5000, HitObjectType,
		false, TArray<AActor*>(), EDrawDebugTrace::None, OutHit, true))
	{
		return OutHit.Location + FVector::UpVector * Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}
	// Set mouse hit location from mouse position and at the Z-height of the player
	else
	{
		FVector MouseLocationInAir = MouseLocation + MouseRotation.Vector() * 1000;
		MouseLocationInAir.Z = Character->GetActorLocation().Z;
		return MouseLocationInAir + FVector::UpVector * Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}
}

FVector ABlindEyePlayerController::GetMouseAimLocation()
{
	FVector MouseLocation;
	FVector MouseRotation;
	DeprojectMousePositionToWorld(OUT MouseLocation, OUT MouseRotation);

	if (GetCharacter() && GetWorld())
	{
		return GetMouseAimLocationHelper(MouseLocation, MouseRotation.Rotation(), GetCharacter(), GetWorld(), MouseAimingObjectTypes);
	}
	return FVector::ZeroVector;
}
