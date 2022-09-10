// Copyright (C) Nicholas Johnson 2022


#include "Enemies/SnapperEnemyController.h"

#include "Shrine.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/SnapperEnemy.h"
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
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp == nullptr) return;
	
	GetBlackboardComponent()->SetValueAsObject(TEXT("EnemyActor"), target);
	Target = MakeWeakObjectPtr<AActor>(target);
}

bool ASnapperEnemyController::CanBasicAttack()
{
	return !IsBasicAttackOnDelay;
}

bool ASnapperEnemyController::IsInBasicAttackRange()
{
	if (Target.IsValid())
	{
		FVector TargetLocation = Target->GetActorLocation();

		if (!Snapper) return false;
		return FVector::Distance(TargetLocation, Snapper->GetActorLocation()) < DistanceToBasicAttack;
	}
	return false;
}

void ASnapperEnemyController::PerformBasicAttack()
{
	Snapper->PerformBasicAttack();
	IsBasicAttackOnDelay = true;
	GetWorldTimerManager().SetTimer(BasicAttackDelayTimerHandle, this, &ASnapperEnemyController::SetCanBasicAttack, BasicAttackDelay, false);
}

void ASnapperEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	Snapper = Cast<ASnapperEnemy>(InPawn);
}

void ASnapperEnemyController::SetCanBasicAttack()
{
	IsBasicAttackOnDelay = false;
}
