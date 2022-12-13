// Copyright (C) Nicholas Johnson 2022


#include "Animations/AnimNotify_SnapperSpawn.h"

#include "Enemies/Burrower/BurrowerEnemy.h"

void UAnimNotify_SnapperSpawn::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	ABurrowerEnemy* Burrower = Cast<ABurrowerEnemy>(MeshComp->GetOwner());
	if (Burrower == nullptr) return;

	Burrower->SpawnSnappers();
}
