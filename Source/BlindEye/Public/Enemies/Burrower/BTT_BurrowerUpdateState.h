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

	UPROPERTY(EditAnywhere)
	uint32 bHidden : 1;

	UPROPERTY(EditAnywhere) 
	uint32 bGravity : 1;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionEnabled::Type> Collision;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
