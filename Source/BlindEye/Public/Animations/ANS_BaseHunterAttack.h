// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Enemies/Hunter/HunterEnemy.h"
#include "ANS_BaseHunterAttack.generated.h"

UCLASS(Abstract)
class BLINDEYE_API UANS_BaseHunterAttack : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	virtual void TryAttack(TArray<FHitResult> Hits);

protected:

	UPROPERTY()
	TSet<AActor*> HitActors;

	UPROPERTY()
	TWeakObjectPtr<AHunterEnemy> Hunter;

	bool bPlayerHit = false;	// Store if a player has already been hit

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	virtual void ApplyHit(FHitResult Hit, bool bApplyMark) PURE_VIRTUAL(UANS_BaseHunterAttack,);
	
};
