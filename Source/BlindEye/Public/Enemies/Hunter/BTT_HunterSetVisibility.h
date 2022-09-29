// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_HunterSetVisibility.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_HunterSetVisibility : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool TurnIsVisible;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
