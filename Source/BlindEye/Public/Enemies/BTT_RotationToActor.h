// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_RotationToActor.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_RotationToActor : public UBTTaskNode
{
	GENERATED_BODY()

	UBTT_RotationToActor();

	UPROPERTY(EditAnywhere, Category=BlackBoard)
	FBlackboardKeySelector TargetActorKey;
	 
	UPROPERTY(EditAnywhere, Category=BlackBoard)
	FBlackboardKeySelector OutRotationKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
