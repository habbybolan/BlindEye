// Copyright (C) Nicholas Johnson 2022


#include "Components/DamageFeedbackComponent.h"

#include "BlindEyeBaseCharacter.h"

UDamageFeedbackComponent::UDamageFeedbackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDamageFeedbackComponent::BeginPlay()
{
	Super::BeginPlay();

	ABlindEyeBaseCharacter* BaseCharacter = Cast<ABlindEyeBaseCharacter>(GetOwner());
	if (ensure(BaseCharacter))
	{
		BaseCharacter->HealthComponent->OnDeathDelegate.AddDynamic(this, &UDamageFeedbackComponent::BP_OnDeath_SER);
		BaseCharacter->HealthComponent->OnDamageDelegate.AddDynamic(this, &UDamageFeedbackComponent::BP_OnTakeDamage_SER);
	}
}

