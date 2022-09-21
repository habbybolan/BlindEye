// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_BurrowerWarningStart.generated.h"

struct FBTBurrowerWaitTaskMemory  
{
	/** time left */
	float RemainingWaitTime;
};

class UNiagaraSystem;
class UNiagaraComponent;


/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTT_BurrowerWarningStart : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UBTT_BurrowerWarningStart();

	UPROPERTY(EditAnywhere)
	float WarningDelay = 3;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual uint16 GetInstanceMemorySize() const override;

	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;
	

protected:
	void EndWarning(UBehaviorTreeComponent& OwnerComp);
	
	void DespawnWarningParticle(UBehaviorTreeComponent& OwnerComp); 
	
};
