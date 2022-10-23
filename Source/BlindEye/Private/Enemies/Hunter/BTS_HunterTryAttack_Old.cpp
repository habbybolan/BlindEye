// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/BTS_HunterTryAttack_Old.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Hunter/HunterEnemyController.h"

UBTS_HunterTryAttack_Old::UBTS_HunterTryAttack_Old()
{
	NodeName = TEXT("Try Attack OLD");
}

void UBTS_HunterTryAttack_Old::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	UObject* TargetObject = BlackboardComponent->GetValueAsObject(TargetKey.SelectedKeyName);
	if (TargetObject == nullptr) return;
	AActor* Target = Cast<AActor>(TargetObject); 
	
	AAIController* AIOwner = OwnerComp.GetAIOwner();
	if (!AIOwner) return;
	AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(AIOwner);
	if (HunterController == nullptr) return;
	
	if (HunterController->CanJumpAttack(Target))
	{
		HunterController->PerformJumpAttack();
	}
}
