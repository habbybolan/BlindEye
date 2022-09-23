// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/BTS_HunterTryAttack.h"

#include "Enemies/Hunter/HunterEnemyController.h"

void UBTS_HunterTryAttack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	// AAIController* AIOwner = OwnerComp.GetAIOwner();
	// if (!AIOwner) return;
	// AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(AIOwner);
	// if (HunterController == nullptr) return;
	//
	// if (HunterController->CanBasicAttack() && HunterController->IsInBasicAttackRange())
	// {
	// 	HunterController->PerformBasicAttack();
	// }
}
