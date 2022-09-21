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
	virtual TEAMS GetTeam_Implementation() override;

	virtual UHealthComponent* GetHealthComponent_Implementation() override;
	virtual UMarkerComponent* GetMarkerComponent_Implementation() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UFUNCTION()
	void OnMarkAdded(PlayerType MarkerType);
	UFUNCTION()
	void OnMarkRemoved();
	UFUNCTION()
	void OnMarkDetonated();

};
