#include "Animations/ANS_BaseHunterAttack.h"

#include "Characters/BlindEyePlayerCharacter.h"

void UANS_BaseHunterAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	Hunter = Cast<AHunterEnemy>(MeshComp->GetOwner());
}

void UANS_BaseHunterAttack::TryAttack(TArray<FHitResult> Hits)
{
	for (FHitResult Hit : Hits)
	{
		if (Hit.Actor == Hunter) continue;
		
		if (!HitActors.Contains(Hit.Actor.Get()))
		{
			HitActors.Add(Hit.Actor.Get());

			// Check if mark should be applied (Hit player and hasn't hit player with attack yet)
			bool bApplyMark = false;
			if (ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(Hit.Actor.Get()))
			{
				if (!bPlayerHit)
				{
					bPlayerHit = true;
					bApplyMark = true;
				}
			}
			ApplyHit(Hit, bApplyMark);
		}
	}
}

void UANS_BaseHunterAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	HitActors.Empty();
	bPlayerHit = false;
}
