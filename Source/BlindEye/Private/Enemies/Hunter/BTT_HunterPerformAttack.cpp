// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/BTT_HunterPerformAttack.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Hunter/HunterEnemyController.h"

UBTT_HunterPerformAttack::UBTT_HunterPerformAttack()
{
	NodeName=TEXT("Perform Attack");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_HunterPerformAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIOwner = OwnerComp.GetAIOwner();
	if (!AIOwner) return EBTNodeResult::Failed;
	AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(AIOwner);
	if (HunterController == nullptr) return EBTNodeResult::Failed;
	Hunter = Cast<AHunterEnemy>(HunterController->GetPawn());

	if (Hunter->GetCurrAttack() > EHunterAttacks::None) return EBTNodeResult::Failed;

	if (AttackType == EHunterAttacks::ChargedJump)
	{
		HunterController->PerformChargedJump();
	} else if (AttackType == EHunterAttacks::BasicAttack)
	{
		HunterController->PerformBasicAttack();
	}
	
	return EBTNodeResult::Succeeded;
}

void UBTT_HunterPerformAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (Hunter == nullptr) return FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	// finished attacking task when hunter stops attacking
	if (!Hunter->GetIsAttacking()) FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
