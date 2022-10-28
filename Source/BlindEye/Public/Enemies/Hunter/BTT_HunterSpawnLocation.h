// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_HunterSpawnLocation.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_HunterSpawnLocation : public UBTTaskNode
{
	GENERATED_BODY()

	UBTT_HunterSpawnLocation();

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector SpawningLocationKey;
	UPROPERTY(EditAnywhere, Category=Blackboard) 
	FBlackboardKeySelector SpawningRotationKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
