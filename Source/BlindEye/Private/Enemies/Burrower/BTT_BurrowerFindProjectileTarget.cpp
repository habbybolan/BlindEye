// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BTT_BurrowerFindProjectileTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Burrower/BurrowerEnemy.h"

EBTNodeResult::Type UBTT_BurrowerFindProjectileTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!BBComp) return EBTNodeResult::Failed;

	EBTNodeResult::Type Result = EBTNodeResult::Failed;

	if (AAIController* Controller = OwnerComp.GetAIOwner())
	{
		if (Controller->GetPawn())
		{
			ABurrowerEnemy* Burrower = Cast<ABurrowerEnemy>(Controller->GetPawn());
			if (AActor* ProjectileTarget = Burrower->GetProjectileTargetActor())
			{
				BBComp->SetValueAsObject(ProjectileTargetKey.SelectedKeyName, ProjectileTarget);
				Result = EBTNodeResult::Succeeded;
			}
		}
	}
	return Result;
}
