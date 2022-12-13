// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Snapper/BTS_TryAttack.h"

#include "Shrine.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Enemies/Snapper/SnapperEnemyController.h"

void UBTS_TryAttack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIOwner = OwnerComp.GetAIOwner();
	if (!AIOwner) return;
	ASnapperEnemyController* SnapperController = Cast<ASnapperEnemyController>(AIOwner);
	if (SnapperController == nullptr) return;

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	
	UObject* TargetObject = BBComp->GetValueAsObject(TargetKey.SelectedKeyName);
	if (TargetObject == nullptr) return;
	AActor* Target = Cast<AActor>(TargetObject);

	SnapperController->SetFocus(Target);

	// perform different ability based on if attacking shrine vs player
	if (SnapperAttack == ESnapperAttacks::BasicAttack)
	{
		if (SnapperController->CanBasicAttack(Target))
		{
			SnapperController->PerformBasicAttack();
		}
	}
	else if (SnapperAttack == ESnapperAttacks::JumpAttack)
	{
		if (SnapperController->CanJumpAttack(Target))
		{
			SnapperController->PerformJumpAttack();
		}
	}
}
