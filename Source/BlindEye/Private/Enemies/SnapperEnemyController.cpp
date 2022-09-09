// Copyright (C) Nicholas Johnson 2022


#include "Enemies/SnapperEnemyController.h"

#include "Shrine.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void ASnapperEnemyController::BeginPlay()
{
	Super::BeginPlay();

	InitializeBehaviorTree();
	
	UWorld* world = GetWorld();
	if (!world) return;
	
	AActor* ShrineActor = UGameplayStatics::GetActorOfClass(world, AShrine::StaticClass());
	
	if (ShrineActor)
	{
		SetTargetEnemy(ShrineActor);
	}
}

void ASnapperEnemyController::SetTargetEnemy(AActor* target)
{
	GetBlackboardComponent()->SetValueAsObject(TEXT("EnemyActor"), target);
	Target = MakeWeakObjectPtr<AActor>(target);
}
