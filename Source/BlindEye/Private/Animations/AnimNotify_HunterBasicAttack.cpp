// Copyright (C) Nicholas Johnson 2022


#include "Animations/AnimNotify_HunterBasicAttack.h"

void UAnimNotify_HunterBasicAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.f, FColor::Red, "SWIPED");
}
