// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Enemies/Hunter/HunterEnemy.h"
#include "ANS_HunterBasicAttack.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UANS_HunterBasicAttack : public UAnimNotifyState
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

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
};
