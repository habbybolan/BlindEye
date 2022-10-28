// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "ANS_BaseHunterAttack.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_HunterChargedJump.generated.h"

class AHunterEnemy;
/**
 * 
 */
UCLASS()
class BLINDEYE_API UANS_HunterChargedJump : public UANS_BaseHunterAttack
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> DamageObjectTypes;

	UPROPERTY(EditDefaultsOnly)
	float Radius = 10.f;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;

	void PerformSwing(FName BoneNameHand, FName BoneNameForeArm, USkeletalMeshComponent* MeshComp);

	virtual void ApplyHit(FHitResult Hit, bool bApplyMark) override;
	
};
