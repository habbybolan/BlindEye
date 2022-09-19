// Copyright (C) Nicholas Johnson 2022


#include "Enemies/BTT_MoveToLocationImmediate.h"

#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTT_MoveToLocationImmediate::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	UObject* SelfObject = BBComp->GetValueAsObject(SelfKey.SelectedKeyName);
	if (SelfObject)
	{
		if (AActor* SelfActor = Cast<AActor>(SelfObject))
		{
			FVector LocationToMove = BBComp->GetValueAsVector(LocationKey.SelectedKeyName);
			SelfActor->SetActorLocation(LocationToMove);
			FRotator RotationOnMove = BBComp->GetValueAsRotator(RotationKey.SelectedKeyName);
			SelfActor->SetActorRotation(RotationOnMove);
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}
