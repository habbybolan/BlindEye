// Copyright (C) Nicholas Johnson 2022


#include "Enemies/BTT_KillEnemy.h"

#include "AIController.h"
#include "Components/HealthComponent.h"
#include "Enemies/BlindEyeEnemyBase.h"
#include "Interfaces/HealthInterface.h"

EBTNodeResult::Type UBTT_KillEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = EBTNodeResult::Failed;

	if (OwnerComp.GetAIOwner())
	{
		if (APawn* Pawn =  OwnerComp.GetAIOwner()->GetPawn())
		{
			if (ABlindEyeEnemyBase* Enemy = Cast<ABlindEyeEnemyBase>(Pawn))
			{
				Enemy->GetHealthComponent()->Kill();
				Result = EBTNodeResult::Succeeded;
			}
		}
	}
	return Result;
}
