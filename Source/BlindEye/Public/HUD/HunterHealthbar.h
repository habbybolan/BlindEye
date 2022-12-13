// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enemies/Hunter/HunterEnemy.h"
#include "HunterHealthbar.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UHunterHealthbar : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AHunterEnemy> Hunter;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHealth(float Percent);

	void SubscribeToHunter(AHunterEnemy* hunter);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_StartAnimation();

	UFUNCTION()
	void HunterHealthUpdated(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser);
};
