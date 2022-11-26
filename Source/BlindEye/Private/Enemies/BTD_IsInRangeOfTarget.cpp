// Copyright (C) Nicholas Johnson 2022


#include "Enemies/BTD_IsInRangeOfTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Hunter/HunterEnemy.h"

UBTD_IsInRangeOfTarget::UBTD_IsInRangeOfTarget()
{
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTD_IsInRangeOfTarget, BlackboardKey), AActor::StaticClass());
}

bool UBTD_IsInRangeOfTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bHasReached = false;
	const UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent(); 

	if (AAIController* AIOwner = OwnerComp.GetAIOwner())
	{
		if (APawn* OwnerPawn = AIOwner->GetPawn())
		{
			UObject* TargetObject = BBComp->GetValueAsObject(BlackboardKey.SelectedKeyName);
			if (TargetObject)
			{
				AActor* TargetActor = Cast<AActor>(TargetObject);
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Cyan,
					FString::SanitizeFloat(FVector::Distance(OwnerPawn->GetActorLocation(), TargetActor->GetActorLocation())));
				bHasReached = FVector::Distance(OwnerPawn->GetActorLocation(), TargetActor->GetActorLocation()) <= AcceptableRadius;
			}
		}
	}
	return bHasReached;
}
