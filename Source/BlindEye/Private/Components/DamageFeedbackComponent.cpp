// Copyright (C) Nicholas Johnson 2022


#include "Components/DamageFeedbackComponent.h"

#include "BlindEyeBaseCharacter.h"
#include "Animation/SkeletalMeshActor.h"

UDamageFeedbackComponent::UDamageFeedbackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDamageFeedbackComponent::BeginPlay()
{
	Super::BeginPlay();

	ABlindEyeEnemyBase* BaseCharacter = Cast<ABlindEyeEnemyBase>(GetOwner());
	check(BaseCharacter)
	
	Enemy = BaseCharacter;
	BaseCharacter->HealthComponent->OnDeathDelegate.AddDynamic(this, &UDamageFeedbackComponent::BP_OnDeath_SER);
	BaseCharacter->HealthComponent->OnDamageDelegate.AddDynamic(this, &UDamageFeedbackComponent::BP_OnTakeDamage_SER);
}

USkeletalMeshComponent* UDamageFeedbackComponent::GetMesh()
{
	return Enemy->GetMesh();
}

