// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/HealthComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/HealthInterface.h"
#include "BlindEyeEnemyBase.generated.h"

UCLASS()
class BLINDEYE_API ABlindEyeEnemyBase : public ACharacter, public IHealthInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABlindEyeEnemyBase();

	// helper to get status effects from health component
	UFUNCTION(BlueprintCallable)
	const FAppliedStatusEffects& GetAppliedStatusEffects(); 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UHealthComponent* HealthComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHealth = 100.f;
	UPROPERTY(Replicated)
	float CurrHealth;

	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;

	// Status effect delegate listeners ******
	
	UFUNCTION()
	virtual void OnStunStart(float StunDuration);
	UFUNCTION()
	virtual void OnStunEnd();

	UFUNCTION()
	virtual void OnBurnStart(float DamagePerSec, float Duration);
	UFUNCTION() 
	virtual void OnBurnEnd();

	UFUNCTION()
	virtual void OnTauntStart(float Duration, AActor* Taunter);
	UFUNCTION()  
	virtual void OnTauntEnd(); 

	// End Status effect delegate listeners ******
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float GetHealth_Implementation() override;
	virtual void SetHealth_Implementation(float NewHealth) override;

	virtual void OnTakeDamage_Implementation(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser) override;
	virtual UHealthComponent* GetHealthComponent_Implementation() override;

	virtual void OnDeath_Implementation() override;
	virtual float GetHealthPercent_Implementation() override;
	
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEAMS Team;
	virtual TEAMS GetTeam_Implementation() override;

	
};
