// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/BTD_HunterTargetMarked.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Enemies/Hunter/HunterEnemy.h"

UBTD_HunterTargetMarked::UBTD_HunterTargetMarked()
{
	NodeName = "Is Target Marked";
	bNotifyTick = false;
}

bool UBTD_HunterTargetMarked::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BBComp))
	{
		if (ABlindEyeBaseCharacter* Player = Cast<ABlindEyePlayerCharacter>(BBComp->GetValueAsObject(EnemyActorKey.SelectedKeyName)))
		{
			FMarkData MarkerData = Player->GetHealthComponent()->GetCurrMark();
			if (MarkerData.bHasMark)
			{
				return MarkerData.MarkerType == EMarkerType::Hunter;
			}
		}
	}
	return false;
}
