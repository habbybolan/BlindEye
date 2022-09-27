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

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	UObject* TargetObject = BlackboardComponent->GetValueAsObject(TargetKey.SelectedKeyName);
	if (TargetObject == nullptr) return;
	AActor* Target = Cast<AActor>(TargetObject); 

	// perform different ability based on if attacking shrine vs player
	if (AShrine* Shrine = Cast<AShrine>(Target))
	{
		if (SnapperController->CanBasicAttack(Shrine))
		{
			SnapperController->PerformBasicAttack();
		}
	} else
	{
		if (SnapperController->CanJumpAttack(Target))
		{
			SnapperController->PerformJumpAttack();
		}
	}
}
