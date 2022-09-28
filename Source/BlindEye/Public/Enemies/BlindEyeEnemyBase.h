// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BlindEyeBaseCharacter.h"
#include "Components/HealthComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/HealthInterface.h"
#include "BlindEyeEnemyBase.generated.h"
 
class UMarkerComponent;

UCLASS()
class BLINDEYE_API ABlindEyeEnemyBase : public ABlindEyeBaseCharacter
{
	GENERATED_BODY()

	

public:

	ABlindEyeEnemyBase(const FObjectInitializer& ObjectInitializer);

	// helper to get status effects from health component
	UFUNCTION(BlueprintCallable)
	const FAppliedStatusEffects& GetAppliedStatusEffects(); 

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHealth = 100.f;
	UPROPERTY(Replicated)
	float CurrHealth;

	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
 
	virtual float GetHealth() override;
	virtual void SetHealth(float NewHealth) override;

	virtual void MYOnTakeDamage(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser) override;

	virtual void OnDeath(AActor* ActorThatKilled) override;
	virtual float GetHealthPercent() override;
	virtual float GetMaxHealth() override;
	
};
