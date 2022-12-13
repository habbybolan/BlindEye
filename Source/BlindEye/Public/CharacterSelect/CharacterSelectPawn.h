// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BlindEyeBaseCharacter.h"
#include "GameFramework/Pawn.h"
#include "CharacterSelectPawn.generated.h"

UCLASS()
class BLINDEYE_API ACharacterSelectPawn : public APawn
{
	GENERATED_BODY()

public:
	ACharacterSelectPawn();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> CharacterModelObjectType;

	UFUNCTION()
	void PlayerTrySelectCharacter();

	UFUNCTION(Server, Reliable)
	void SER_PlayerSelectHelper(EPlayerType CharacterSelected);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
