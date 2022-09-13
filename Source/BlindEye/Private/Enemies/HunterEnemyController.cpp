// Copyright (C) Nicholas Johnson 2022


#include "Enemies/HunterEnemyController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void AHunterEnemyController::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeBehaviorTree();
	
	UWorld* world = GetWorld();
	if (!world) return;

	// Get random playerto attack
	AGameStateBase* GameState = UGameplayStatics::GetGameState(world);
	AActor* RandPlayerTarget;
	if (GameState->PlayerArray.Num() == 1)
	{
		RandPlayerTarget = GameState->PlayerArray[0]->GetPawn();
	}
	else
	{
		RandPlayerTarget = GameState->PlayerArray[UKismetMathLibrary::RandomIntegerInRange(0, 1)]->GetPawn();
	}
	SetTargetEnemy(RandPlayerTarget);
}

void AHunterEnemyController::SetTargetEnemy(AActor* target)
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp == nullptr) return;
	
	GetBlackboardComponent()->SetValueAsObject(TEXT("EnemyActor"), target);
	Target = MakeWeakObjectPtr<AActor>(target);
}

bool AHunterEnemyController::CanBasicAttack()
{
	return !IsBasicAttackOnDelay;
}

bool AHunterEnemyController::IsInBasicAttackRange()
{
	if (Target.IsValid())
	{
		FVector TargetLocation = Target->GetActorLocation();

		if (!Hunter) return false;
		return FVector::Distance(TargetLocation, Hunter->GetActorLocation()) < DistanceToBasicAttack;
	}
	return false;
}

void AHunterEnemyController::PerformBasicAttack()
{
	if (!CanBasicAttack()) return;
	
	Hunter->PerformBasicAttack();
	IsBasicAttackOnDelay = true;
	GetWorldTimerManager().SetTimer(BasicAttackDelayTimerHandle, this, &AHunterEnemyController::SetCanBasicAttack, BasicAttackDelay, false);
}

void AHunterEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	Hunter = Cast<AHunterEnemy>(InPawn);
}

void AHunterEnemyController::SetCanBasicAttack()
{
	IsBasicAttackOnDelay = false;
}
