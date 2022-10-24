// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/BTS_HunterState.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Hunter/HunterEnemyController.h"

UBTS_HunterState::UBTS_HunterState()
{
	NodeName = TEXT("Hunter State");
	bNotifyTick = true;
}

void UBTS_HunterState::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	AAIController* Controller = SearchData.OwnerComp.GetAIOwner();
	HunterController = Cast<AHunterEnemyController>(Controller);
	Hunter = Cast<AHunterEnemy>(HunterController->GetPawn());
}

void UBTS_HunterState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BBComp))
	{
		BBComp->SetValueAsBool(IsAttackingKey.SelectedKeyName, Hunter->GetIsAttacking());
		BBComp->SetValueAsBool(bChargedAttackOnCooldownKey.SelectedKeyName, Hunter->GetIsChargedAttackOnCooldown());

		FAppliedStatusEffects StatusEffects = Hunter->GetAppliedStatusEffects();
		BBComp->SetValueAsBool(IsImmobilizedKey.SelectedKeyName, StatusEffects.IsStun || StatusEffects.IsStaggered);
	}
}
