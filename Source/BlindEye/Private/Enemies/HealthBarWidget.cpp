// Copyright (C) Nicholas Johnson 2022


#include "Enemies/HealthBarWidget.h"

#include "Interfaces/HealthInterface.h"

void UHealthBarWidget::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthBarWidget::OnTakeDamage);
}

void UHealthBarWidget::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (const IHealthInterface* HealthInterface = Cast<IHealthInterface>(GetOwner()))
	{
		HealthInterface->Execute_GetHealthPercent(GetOwner());
	}
}

void UHealthBarWidget::VisibilityCheck()
{
	// TODO:
}
