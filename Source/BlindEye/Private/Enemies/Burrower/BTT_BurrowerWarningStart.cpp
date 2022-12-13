// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BTT_BurrowerWarningStart.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Enemies/Burrower/BurrowerEnemyController.h"

UBTT_BurrowerWarningStart::UBTT_BurrowerWarningStart()
{
	NodeName = TEXT("Warning");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_BurrowerWarningStart::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UWorld* world = GetWorld();
	if (world == nullptr) return EBTNodeResult::Failed;

	FBTBurrowerWaitTaskMemory* MyMemory = (FBTBurrowerWaitTaskMemory*)NodeMemory;
	MyMemory->RemainingWaitTime = WarningDelay;

	AAIController* Controller = OwnerComp.GetAIOwner();
	if (ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller))
	{
		BurrowerController->StartWarningParticles();
	}
	
	
	return EBTNodeResult::InProgress;
}

void UBTT_BurrowerWarningStart::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTBurrowerWaitTaskMemory* MyMemory = (FBTBurrowerWaitTaskMemory*)NodeMemory;
	MyMemory->RemainingWaitTime -= DeltaSeconds;

	if (MyMemory->RemainingWaitTime <= 0.0f)
	{
		// continue execution from this node
		MyMemory->RemainingWaitTime = 0.0f;
		EndWarning(OwnerComp);
	}
}

uint16 UBTT_BurrowerWarningStart::GetInstanceMemorySize() const
{
	return sizeof(FBTBurrowerWaitTaskMemory);
}

EBTNodeResult::Type UBTT_BurrowerWarningStart::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller))
	{
		BurrowerController->StopWarningParticles();
	}
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTT_BurrowerWarningStart::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                      EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);

	FBTBurrowerWaitTaskMemory* MyMemory = (FBTBurrowerWaitTaskMemory*)NodeMemory;
	if (MyMemory->RemainingWaitTime)
	{
		Values.Add(FString::Printf(TEXT("remaining: %ss"), *FString::SanitizeFloat(MyMemory->RemainingWaitTime)));
	}
}

void UBTT_BurrowerWarningStart::EndWarning(UBehaviorTreeComponent& OwnerComp)
{
	DespawnWarningParticle(OwnerComp);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

void UBTT_BurrowerWarningStart::DespawnWarningParticle(UBehaviorTreeComponent& OwnerComp)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller))
	{
		BurrowerController->StopWarningParticles();
	}
}
