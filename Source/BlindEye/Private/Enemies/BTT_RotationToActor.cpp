// Copyright (C) Nicholas Johnson 2022


#include "Enemies/BTT_RotationToActor.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_RotationToActor::UBTT_RotationToActor()
{
	NodeName = TEXT("Rotation To Actor");
	bNotifyTick = false;
}

EBTNodeResult::Type UBTT_RotationToActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AActor* selfActor = OwnerComp.GetAIOwner()->GetPawn();
	if (selfActor == nullptr) return EBTNodeResult::Failed;
	
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	UObject* ActorObject = BBComp->GetValueAsObject(TargetActorKey.SelectedKeyName);
	if (ActorObject)
	{ 
		if (AActor* targetActor = Cast<AActor>(ActorObject))
		{
			FRotator outRotation = (targetActor->GetActorLocation() - selfActor->GetActorLocation()).Rotation();
			BBComp->SetValueAsRotator(OutRotationKey.SelectedKeyName, outRotation);
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}
