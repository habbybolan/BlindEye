// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Enemies/BlindEyeEnemyController.h"
#include "SnapperEnemyController.generated.h"

class UBehaviorTree;

/**
 * 
 */
UCLASS()
class BLINDEYE_API ASnapperEnemyController : public ABlindEyeEnemyController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	// Calls blueprint to initialize behavior tree
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeBehaviorTree();
	
	void SetTargetEnemy(AActor* target);
	
protected:
	TWeakObjectPtr<AActor> Target;
	
	
};
