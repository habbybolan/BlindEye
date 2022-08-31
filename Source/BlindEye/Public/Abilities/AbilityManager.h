// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "AbilityManager.generated.h"

// represents what type of actions the current ability's state is blocking
enum class AbilityBlockers
{
	IsBlockMovement = 0,
	IsBlockAbility = 1,
	IsBlockDamageFeedback = 2,
	IsBlockReceiveDamage = 3
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLINDEYE_API UAbilityManager : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilityManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
