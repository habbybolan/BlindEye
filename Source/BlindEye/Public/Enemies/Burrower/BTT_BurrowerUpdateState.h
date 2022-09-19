// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_BurrowerUpdateState.generated.h"

/**
 * 
 */
UCLASS(config=Game)
class BLINDEYE_API UBTT_BurrowerUpdateState : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UBTT_BurrowerUpdateState();

	UPROPERTY(EditAnywhere)
	uint32 bHidden : 1;

	// Sets if the collision preset for if burrower if following the player or not
	UPROPERTY(EditAnywhere)
	uint32 bFollowing : 1;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
