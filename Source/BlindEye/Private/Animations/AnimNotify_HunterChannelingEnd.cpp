// Copyright (C) Nicholas Johnson 2022


#include "Animations/AnimNotify_HunterChannelingEnd.h"

#include "Enemies/Hunter/HunterEnemy.h"

void UAnimNotify_HunterChannelingEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AHunterEnemy* Hunter = Cast<AHunterEnemy>(MeshComp->GetOwner());
	check(Hunter);

	Hunter->ChannelingAnimFinished();
}
