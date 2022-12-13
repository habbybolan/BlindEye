// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlindEyePlayerController.generated.h"

class ABlindEyePlayerCharacter;
/**
 * 
 */
UCLASS()
class BLINDEYE_API ABlindEyePlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category=TopdownAim)
	TArray<TEnumAsByte<EObjectTypeQuery>> MouseAimingObjectTypes;

	UFUNCTION(Server, Reliable)
	void SER_SpawnPlayer();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SER_RestartLevel();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SER_CharacterSelect();

	virtual void OnPossess(APawn* InPawn) override;

	FRotator GetDesiredRotationFromMouse();

	static FVector GetMouseAimLocationHelper(FVector MouseLocation, FRotator MouseRotation, ACharacter* Character,
		UWorld* World, TArray<TEnumAsByte<EObjectTypeQuery>> HitObjectType = TArray<TEnumAsByte<EObjectTypeQuery>>());
	FVector GetMouseAimLocation();

private:
	UPROPERTY()
	ABlindEyePlayerCharacter* OwningCharacter;
	
};

// TODO: Temporary for test spawning
static bool IsServer = true;
