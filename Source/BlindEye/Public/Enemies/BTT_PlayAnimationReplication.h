// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_PlayAnimationReplication.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_PlayAnimationReplication : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	UAnimMontage* AnimToPlay;

	UPROPERTY()
	UBehaviorTreeComponent* MyOwnerComp;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void OnAnimEnded(UAnimMontage* Montage, bool bInterrupted);

protected:
	void CleanUp(UBehaviorTreeComponent& OwnerComp);
};
