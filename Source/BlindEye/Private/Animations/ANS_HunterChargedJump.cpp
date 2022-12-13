// Copyright (C) Nicholas Johnson 2022


#include "Animations/ANS_HunterChargedJump.h"

#include "Characters/BlindEyePlayerCharacter.h"
#include "Enemies/Hunter/HunterEnemy.h"
#include "Kismet/KismetSystemLibrary.h"

void UANS_HunterChargedJump::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	if (!Hunter.IsValid()) return;
 
	PerformSwing(TEXT("LeftHand"), TEXT("LeftForeArm"), MeshComp);
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
		EDrawDebugTrace::None, HitResults, true))
	{
		TryAttack(HitResults);
	}
}

void UANS_HunterChargedJump::ApplyHit(FHitResult Hit, bool bApplyMark)
{
	Hunter->ApplyChargedJumpDamage(Hit, bApplyMark);
}
