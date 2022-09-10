// Copyright (C) Nicholas Johnson 2022


#include "Enemies/BTS_TryAttack.h"

#include "Enemies/SnapperEnemyController.h"

void UBTS_TryAttack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIOwner = OwnerComp.GetAIOwner();
	if (!AIOwner) return;
	ASnapperEnemyController* SnapperController = Cast<ASnapperEnemyController>(AIOwner);
	if (SnapperController == nullptr) return;

	if (SnapperController->CanBasicAttack() && SnapperController->IsInBasicAttackRange())
	{
		SnapperController->PerformBasicAttack();
	}
	
}
