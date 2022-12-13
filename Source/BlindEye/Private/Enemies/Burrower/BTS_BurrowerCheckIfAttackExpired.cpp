// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BTS_BurrowerCheckIfAttackExpired.h"

#include "BehaviorTree/BlackboardComponent.h"

void UBTS_BurrowerCheckIfAttackExpired::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                 float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	CurrTimer += DeltaSeconds;

	if (CurrTimer >= AttackExpiredTime)
	{
		UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
		BBComp->SetValueAsBool(IsAttackingExpiredKey.SelectedKeyName, true);
		CurrTimer = 0;
	}
}
