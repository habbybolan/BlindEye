// Copyright (C) Nicholas Johnson 2022


#include "Enemies/BTS_TryAttack.h"

#include "Shrine.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Enemies/SnapperEnemyController.h"

void UBTS_TryAttack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIOwner = OwnerComp.GetAIOwner();
	if (!AIOwner) return;
	ASnapperEnemyController* SnapperController = Cast<ASnapperEnemyController>(AIOwner);
	if (SnapperController == nullptr) return;

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	UObject* TargetObject = BlackboardComponent->GetValueAsObject(TargetKey.SelectedKeyName);
	if (TargetObject == nullptr) return;
	AActor* Target = Cast<AActor>(TargetObject);

	if (const AShrine* Shrine = Cast<AShrine>(Target))
	{
		
	} else
	{
		if (SnapperController->CanJumpAttack() && SnapperController->IsInJumpAttackRange(Target))
		{
			SnapperController->PerformJumpAttack();
		}
	}
	
}
