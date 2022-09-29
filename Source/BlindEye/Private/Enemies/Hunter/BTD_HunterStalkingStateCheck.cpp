// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/BTD_HunterStalkingStateCheck.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTD_HunterStalkingStateCheck::UBTD_HunterStalkingStateCheck()
{
	NodeName = "Hunter Stalking State Check";
	bNotifyTick = false;
}

void UBTD_HunterStalkingStateCheck::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	if (UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent())
	{
		BBComp->SetValueAsBool(bDamagedKey.SelectedKeyName, false);
		BBComp->SetValueAsBool(bStalkingStrafeKey.SelectedKeyName, true);
	}
}

void UBTD_HunterStalkingStateCheck::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
	if (UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent())
	{
		BBComp->SetValueAsBool(bDamagedKey.SelectedKeyName, false);
		BBComp->SetValueAsBool(bStalkingStrafeKey.SelectedKeyName, true);
	}
}
