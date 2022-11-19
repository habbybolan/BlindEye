// Copyright (C) Nicholas Johnson 2022


#include "Animations/ANS_SnapperJumpAttackHitCheck.h"

#include "Enemies/BlindEyeEnemyController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UANS_SnapperJumpAttackHitCheck::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                 float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	Snapper = MakeWeakObjectPtr(Cast<ASnapperEnemy>(MeshComp->GetOwner()));
}

void UANS_SnapperJumpAttackHitCheck::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	if (!Snapper.IsValid()) return;
	if (MeshComp->GetOwner()->GetLocalRole() < ROLE_Authority) return;
	
	UWorld* world = MeshComp->GetWorld();
	if (!world) return;
	
	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects(world, Snapper->GetActorLocation(),Snapper->GetActorLocation() + Snapper->GetActorForwardVector() * AttackTraceBoxLength,
		FVector(0, AttackTraceBoxWidth, AttackTraceBoxWidth),
		Snapper->GetActorRotation(), PlayerObjectType, false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, OutHits, true);

	// If hit player, stop jumping attack and apply damage
	for (FHitResult Hit : OutHits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) continue;
	
		UGameplayStatics::ApplyPointDamage(HitActor, JumpAttackDamage, Hit.ImpactNormal, Hit, Snapper->GetController(), Snapper.Get(), JumpAttackDamageType);
		Snapper->StopJumpAttack();
	}
}
