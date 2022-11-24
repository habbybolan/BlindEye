// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_CharacterSelectBoids.generated.h"

class ABasicAttackSmallFlock;
/**
 * 
 */
UCLASS()
class BLINDEYE_API UAN_CharacterSelectBoids : public UAnimNotify
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ABasicAttackSmallFlock> FlockType;

	UPROPERTY(EditDefaultsOnly)
	bool bRightHand = true;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
