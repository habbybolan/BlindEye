// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_HunterChargedJump.generated.h"

class AHunterEnemy;
/**
 * 
 */
UCLASS()
class BLINDEYE_API UANS_HunterChargedJump : public UAnimNotifyState
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> DamageObjectTypes;

	UPROPERTY(EditDefaultsOnly)
	float Radius = 10.f;

	UPROPERTY()
	TSet<AActor*> HitActors;

	UPROPERTY()
	TWeakObjectPtr<AHunterEnemy> Hunter;

	bool bPlayerHit = false;	// Store if a player has already been hit

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	void PerformSwing(FName BoneNameHand, FName BoneNameForeArm, USkeletalMeshComponent* MeshComp);
	
};
