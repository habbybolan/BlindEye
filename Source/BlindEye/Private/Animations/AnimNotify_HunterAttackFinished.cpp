// Copyright (C) Nicholas Johnson 2022


#include "Animations/AnimNotify_HunterAttackFinished.h"

#include "Enemies/Hunter/HunterEnemy.h"

void UAnimNotify_HunterAttackFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AHunterEnemy* Hunter = Cast<AHunterEnemy>(MeshComp->GetOwner());
	if (!Hunter) return;

	Hunter->SetAttackFinished();
}
