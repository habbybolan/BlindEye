// Copyright (C) Nicholas Johnson 2022


#include "Enemies/BTD_CheckInRangeToAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/BlindEyeEnemybase.h"
#include "Enemies/BlindEyeEnemyController.h"

bool UBTD_CheckInRangeToAttack::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == NULL) return false;
	
	FVector TargetLocation;
	UObject* TargetObject = BlackboardComp->GetValueAsObject(Target.SelectedKeyName);
	if (TargetObject)
	{
		AActor* actor = Cast<AActor>(TargetObject);
		if (actor)
		{
			TargetLocation = actor->GetActorLocation();
		} else return false;

		const AAIController* AIOwner = OwnerComp.GetAIOwner();
		if (!AIOwner) return false;
		const APawn* Pawn = AIOwner->GetInstigator();
		if (!Pawn) return false;

		return FVector::Distance(TargetLocation, Pawn->GetActorLocation()) < DistanceToAttack;
	}
	return false;
}
