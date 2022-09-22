// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "HealthBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UHealthBarWidget : public UWidgetComponent
{
	GENERATED_BODY()

public:
 
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHealthBar(float NewHealthPercent);
	
protected:
	virtual void BeginPlay() override;

	void OnTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


	// Check players on
	void VisibilityCheck();
	FTimerHandle VisibilityTimerHandle;
};
