// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_SnapperBasicAttack.generated.h"

class UBaseDamageType;
/**
 * 
 */
UCLASS()
class BLINDEYE_API UAN_SnapperBasicAttack : public UAnimNotify
{
	GENERATED_BODY()

public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BasicAttackDamage = 5; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UBaseDamageType> BasicAttackDamageType;

	UPROPERTY(EditDefaultsOnly) 
	TArray<TEnumAsByte<	EObjectTypeQuery>> PlayerObjectType;
	
};
