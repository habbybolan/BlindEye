// Copyright (C) Nicholas Johnson 2022


#include "Animations/AN_BurrowerShootProjectile.h"

#include "Enemies/Burrower/BurrowerEnemy.h"

void UAN_BurrowerShootProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	ABurrowerEnemy* Burrower = Cast<ABurrowerEnemy>(MeshComp->GetOwner());
	if (Burrower)
	{
		Burrower->ShootProjectile();
	}
}
