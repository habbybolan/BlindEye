// Copyright (C) Nicholas Johnson 2022


#include "Animations/GenericAbilityAnimNotify.h"

#include "Abilities/AbilityManager.h"
#include "Characters/BlindEyePlayerCharacter.h"

// Sends generic Anim notify that occurred on animation to ability that called the animation
void UGenericAbilityAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (ABlindEyePlayerCharacter* PlayerCharacter = Cast<ABlindEyePlayerCharacter>(MeshComp->GetOwner()))
	{
		PlayerCharacter->GetAbilityManager()->PerformGenericAbilityNotify();
	}
}
