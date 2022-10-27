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

	UWorld* World = MeshComp->GetWorld();
	if (!ensure(World)) return;

	// Attack from Right arm bone
	FVector SwingStartLOC = MeshComp->GetBoneLocation("RightArm");
	FVector SwingEndLOC = SwingStartLOC + (MeshComp->GetBoneLocation("RightForeArm") - MeshComp->GetBoneLocation("RightArm")) * 3;

	TArray<FHitResult> HitResults;
	if (UKismetSystemLibrary::SphereTraceMultiForObjects(World, SwingStartLOC, SwingEndLOC, Radius, DamageObjectTypes, false, TArray<AActor*>(),
		EDrawDebugTrace::None, HitResults, true))
	{
		TryAttack(HitResults);
	}  
}

void UANS_HunterBasicAttack::ApplyHit(FHitResult Hit, bool bApplyMark)
{
	Hunter->ApplyBasicAttackDamage(Hit, bApplyMark);
}
