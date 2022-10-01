// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/BTT_HunterStrafeInDirection.h"

UBTT_HunterStrafeInDirection::UBTT_HunterStrafeInDirection(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = TEXT("Strafe In Direction");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_HunterStrafeInDirection::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UBTT_HunterStrafeInDirection::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (APawn* pawn = Controller->GetPawn())
	{
		pawn->AddMovementInput(pawn->GetActorRightVector());
		return;
	}
	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}
