// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Snapper/BTS_SnapperState.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Snapper/SnapperEnemy.h"

void UBTS_SnapperState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!BBComp) return;
	
	if (AAIController* Controller = OwnerComp.GetAIOwner())
	{
		if (Controller->GetPawn())
		{
			ASnapperEnemy* Snapper = Cast<ASnapperEnemy>(Controller->GetPawn());

			BBComp->SetValueAsBool(IsRagdolling.SelectedKeyName, Snapper->GetIsRagdolling());
			BBComp->SetValueAsEnum(AttackState.SelectedKeyName, (uint8)Snapper->CurrAttack);
		}
	}
}
