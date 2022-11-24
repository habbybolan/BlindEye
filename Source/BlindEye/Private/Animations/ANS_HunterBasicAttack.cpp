// Copyright (C) Nicholas Johnson 2022


#include "Animations/ANS_HunterBasicAttack.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Enemies/Hunter/HunterEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UANS_HunterBasicAttack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	if (!Hunter.IsValid()) return;

	// choose which arm is swinging
	if (bLeftArmSwing)
	{
		PerformSwing(TEXT("LeftHand"), TEXT("LeftForeArm"), MeshComp);
	} else
	{
		PerformSwing(TEXT("RightHand"), TEXT("RightForeArm"), MeshComp);
	}
}

void UANS_HunterBasicAttack::ApplyHit(FHitResult Hit, bool bApplyMark)
{
	Hunter->ApplyBasicAttackDamage(Hit, bApplyMark);
}

void UANS_HunterBasicAttack::PerformSwing(FName BoneNameHand, FName BoneNameForeArm, USkeletalMeshComponent* MeshComp)
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
		TryAttack(HitResults);
	}
}
