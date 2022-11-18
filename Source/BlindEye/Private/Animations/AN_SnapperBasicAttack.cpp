// Copyright (C) Nicholas Johnson 2022


#include "Animations/AN_SnapperBasicAttack.h"

#include "Enemies/Snapper/SnapperEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UAN_SnapperBasicAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	UWorld* World = MeshComp->GetWorld();
	if (!World) return;
	
	ASnapperEnemy* Snapper = Cast<ASnapperEnemy>(MeshComp->GetOwner());

	if (Snapper)
	{
		TArray<FHitResult> OutHits;
		UKismetSystemLibrary::BoxTraceMultiForObjects(World, Snapper->GetActorLocation(), Snapper->GetActorForwardVector() * 300, FVector(100/2, 100 / 2, 100 / 2),
			Snapper->GetActorRotation(), PlayerObjectType, false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, OutHits, true);
	
		for (FHitResult Hit : OutHits)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor) continue;
	
			UGameplayStatics::ApplyPointDamage(HitActor, BasicAttackDamage
				, Hit.ImpactNormal, Hit, Snapper->GetController(), Snapper, BasicAttackDamageType);
		}
	}
	
}
