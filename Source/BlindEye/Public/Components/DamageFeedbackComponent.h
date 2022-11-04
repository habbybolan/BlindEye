// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Components/ActorComponent.h"
#include "DamageFeedbackComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLINDEYE_API UDamageFeedbackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDamageFeedbackComponent();

protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnDeath_SER(AActor* ActorThatKilled);

	UFUNCTION(BlueprintImplementableEvent) 
	void BP_OnTakeDamage_SER(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser);
};
