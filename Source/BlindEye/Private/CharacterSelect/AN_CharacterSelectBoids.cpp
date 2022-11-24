// Copyright (C) Nicholas Johnson 2022


#include "CharacterSelect/AN_CharacterSelectBoids.h"

#include "Boids/BasicAttackSmallFlock.h"
#include "Kismet/GameplayStatics.h"

void UAN_CharacterSelectBoids::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	UWorld* World = MeshComp->GetWorld();
	if (World) 
	{
		ABasicAttackSmallFlock* Flock = World->SpawnActorDeferred<ABasicAttackSmallFlock>(FlockType, FTransform::Identity, MeshComp->GetOwner(),
			Cast<APawn>(MeshComp->GetOwner()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		Flock->BoneSpawnLocation = bRightHand ? "RightHand" : "LeftHand";
		Flock->InitialTarget = MeshComp->GetOwner()->GetActorLocation() + MeshComp->GetOwner()->GetActorForwardVector() * 5000;
		UGameplayStatics::FinishSpawningActor(Flock, FTransform::Identity);
	}
}
