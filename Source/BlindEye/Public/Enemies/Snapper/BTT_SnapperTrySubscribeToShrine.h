// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SnapperTrySubscribeToShrine.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_SnapperTrySubscribeToShrine : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector ShrineActorKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector ShrineAttackPointKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
