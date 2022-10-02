// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HealthInterface.h"
#include "BlindEyeUtils.h"
#include "BlindEyeBaseCharacter.generated.h"

class UMarkerComponent;
class UHealthComponent;
enum class PlayerType : uint8;

UENUM(BlueprintType)
enum class ECharacterTypes : uint8
{
	Crow,
	Phoenix,
	Snapper,
	Hunter,
	Burrower,
	Other	// Environmental / Debug killed
};	
 
UCLASS(Abstract)
class BLINDEYE_API ABlindEyeBaseCharacter : public ACharacter, public IHealthInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABlindEyeBaseCharacter(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMarkerComponent* MarkerComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEAMS Team; 
	virtual TEAMS GetTeam() override;

	// Notify Blueprint damage taken
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnTakeDamage(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser);

	virtual void MYOnTakeDamage(float Damage, FVector HitLocation, const UDamageType* DamageType, AActor* DamageCauser) override;

	virtual UHealthComponent* GetHealthComponent() override;
	virtual UMarkerComponent* GetMarkerComponent() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ECharacterTypes GetCharacterType(AActor* Character);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnDeath(AActor* ActorThatKilled) override;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnDeath(AActor* ActorThatKilled);

	// mark added
	UFUNCTION()
	void OnMarkAdded(PlayerType MarkerType);
	UFUNCTION(NetMulticast, Reliable)
	void MULT_OnMarkAddedHelper(PlayerType MarkerType);
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnMarkAdded(PlayerType MarkerType);
	// mark removed
	UFUNCTION()
	void OnMarkRemoved();
	UFUNCTION(NetMulticast, Reliable)
	void MULT_OnMarkRemovedHelper();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnMarkRemoved();
	// mark detonated
	UFUNCTION() 
	void OnMarkDetonated();
	UFUNCTION(NetMulticast, Reliable)
	void MULT_OnMarkDetonatedHelper(); 
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnMarkDetonated();
public:
	virtual float GetHealth() override;
	virtual float GetMaxHealth() override;
	virtual void SetHealth(float NewHealth) override;
	virtual float GetHealthPercent() override;
	virtual bool GetIsDead() override;
};
