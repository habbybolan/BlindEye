// Copyright (C) Nicholas Johnson 2022


#include "Animations/ANS_HunterChargedJump.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Enemies/Hunter/HunterEnemy.h"
#include "Kismet/KismetSystemLibrary.h"

void UANS_HunterChargedJump::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	Hunter = Cast<AHunterEnemy>(MeshComp->GetOwner());
}

void UANS_HunterChargedJump::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	if (!Hunter.IsValid()) return;
 
	PerformSwing(TEXT("RightHand"), TEXT("RightForeArm"), MeshComp);
	PerformSwing(TEXT("LeftHand"), TEXT("LeftForeArm"), MeshComp);
}

void UANS_HunterChargedJump::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	HitActors.Empty();
} 

void UANS_HunterChargedJump::PerformSwing(FName BoneNameHand, FName BoneNameForeArm, USkeletalMeshComponent* MeshComp)
{
	UWorld* World = MeshComp->GetWorld();
	if (!ensure(World)) return;

	// Attack from Right arm bone
	FVector SwingStartLOC = MeshComp->GetBoneLocation(BoneNameForeArm);
	FVector SwingEndLOC = SwingStartLOC + (MeshComp->GetBoneLocation(BoneNameHand) - MeshComp->GetBoneLocation(BoneNameForeArm)) * 3;

	TArray<FHitResult> HitResults;
	if (UKismetSystemLibrary::SphereTraceMultiForObjects(World, SwingStartLOC, SwingEndLOC, Radius, DamageObjectTypes, false, TArray<AActor*>(),
		EDrawDebugTrace::ForDuration, HitResults, true))
	{
		for (FHitResult Hit : HitResults)
		{
			if (!HitActors.Contains(Hit.Actor.Get()))
			{
				HitActors.Add(Hit.Actor.Get());

				bool bApplyMark = false;
				if (ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(Hit.Actor.Get()))
				{
					if (!bPlayerHit)
					{
						bPlayerHit = true;
						bApplyMark = true;
					}
				}
				Hunter->ApplyChargedJumpDamage(Hit, bApplyMark);
			}
		}
	}
}
