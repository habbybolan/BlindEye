// Copyright (C) Nicholas Johnson 2022


#include "Animations/ANS_HunterBasicAttack.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Enemies/Hunter/HunterEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UANS_HunterBasicAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	Hunter = Cast<AHunterEnemy>(MeshComp->GetOwner());
}

void UANS_HunterBasicAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	HitActors.Empty();
	bPlayerHit = false;
}

void UANS_HunterBasicAttack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	if (!Hunter.IsValid()) return;

	UWorld* World = MeshComp->GetWorld();
	if (!ensure(World)) return;

	// Attack from Right arm bone
	FVector SwingStartLOC = MeshComp->GetBoneLocation("RightForeArm");
	FVector SwingEndLOC = SwingStartLOC + (MeshComp->GetBoneLocation("RightHand") - MeshComp->GetBoneLocation("RightForeArm")) * 3;

	TArray<FHitResult> HitResults;
	if (UKismetSystemLibrary::SphereTraceMultiForObjects(World, SwingStartLOC, SwingEndLOC, Radius, DamageObjectTypes, false, TArray<AActor*>(),
		EDrawDebugTrace::ForDuration, HitResults, true))
	{
		for (FHitResult Hit : HitResults)
		{
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
				Hunter->ApplyBasicAttackDamage(Hit, bApplyMark);
			}
		}
	}
}
