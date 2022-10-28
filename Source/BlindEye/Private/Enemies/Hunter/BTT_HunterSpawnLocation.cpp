// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/BTT_HunterSpawnLocation.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Burrower/BurrowerSpawnPoint.h"
#include "Kismet/GameplayStatics.h"

UBTT_HunterSpawnLocation::UBTT_HunterSpawnLocation()
{
	NodeName = TEXT("Find Spawning Location");
	bNotifyTick = false;
}

EBTNodeResult::Type UBTT_HunterSpawnLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return EBTNodeResult::Failed;

	TArray<AActor*> OutPoints;
	UGameplayStatics::GetAllActorsOfClass(World, ABurrowerSpawnPoint::StaticClass(), OutPoints);
	for (AActor* Point : OutPoints)
	{
		UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
		BBComp->SetValueAsVector(SpawningLocationKey.SelectedKeyName, Point->GetActorLocation());
		BBComp->SetValueAsRotator(SpawningRotationKey.SelectedKeyName, Point->GetActorRotation());
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
