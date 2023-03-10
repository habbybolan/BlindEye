// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BTS_BurrowerState.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Burrower/BurrowerEnemyController.h"
#include "Kismet/GameplayStatics.h"

void UBTS_BurrowerState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AAIController* Controller = OwnerComp.GetAIOwner();
	ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller);
	check(BurrowerController)

	ABurrowerEnemy* Burrower = Cast<ABurrowerEnemy>(BurrowerController->GetPawn());
	if (!Burrower) return;

	BBComp->SetValueAsBool(IsDeadKey.SelectedKeyName, Burrower->GetIsDead());

	BBComp->SetValueAsEnum(VisibilityStateKey.SelectedKeyName, (uint8)Burrower->GetVisibilityState());

	// Check if attack state finished (All players left island and burrower hidden)
	if (BurrowerController->IsHidden() &&
		BBComp->GetValueAsEnum(StateKey.SelectedKeyName) == (uint8)EBurrowActionState::Attacking &&
		BBComp->GetValueAsObject(EnemyActorKey.SelectedKeyName) == nullptr)
	{
		BBComp->SetValueAsEnum(StateKey.SelectedKeyName, (uint8)EBurrowActionState::Patrolling);
	}

	// Check if Cancelled state finished
	if (BurrowerController->IsHidden() &&
		BBComp->GetValueAsEnum(StateKey.SelectedKeyName) == (uint8)EBurrowActionState::Cancelled)
	{
		BBComp->SetValueAsEnum(StateKey.SelectedKeyName, (uint8)EBurrowActionState::Patrolling);
	}

	EBurrowActionState CurrState = (EBurrowActionState)BBComp->GetValueAsEnum(StateKey.SelectedKeyName);

	// Check if surfacing time should be activated / is queued up
	if (CurrState != EBurrowActionState::Spawning && ( bSpawnStateQueued || CurrSpawnStateTime >= SpawnStateDelay))
	{
		// if attacking, set spawn state as queued
		if (CurrState == EBurrowActionState::Attacking)
		{
			bSpawnStateQueued = true;
		} else if (CurrState == EBurrowActionState::Patrolling)
		{
			bSpawnStateQueued = false;
			CurrSpawnStateTime = 0;
			BBComp->SetValueAsEnum(StateKey.SelectedKeyName, (uint8)EBurrowActionState::Spawning);
		}
	}
	// State check based on EnemyActor being set
	else
	{
		// prevent changing state automatically while in spawning state
		if (CurrState != EBurrowActionState::Spawning && CurrState != EBurrowActionState::Cancelled)
		{
			// If Enemy actor set
			if (BBComp->GetValueAsObject(EnemyActorKey.SelectedKeyName))
			{
				BBComp->SetValueAsEnum(StateKey.SelectedKeyName, (uint8)EBurrowActionState::Attacking);
			}
		}
	}
	
	// Check if time to spawn
	if (CurrState != EBurrowActionState::Spawning)
	{
		CurrSpawnStateTime += DeltaSeconds;
	}
}

void UBTS_BurrowerState::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	UBlackboardComponent* BBComp = SearchData.OwnerComp.GetBlackboardComponent();

	// Set initial values on burrower search start
	ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(SearchData.OwnerComp.GetAIOwner());
	if (BurrowerController->GetPawn())
	{
		ABurrowerEnemy* Burrower = Cast<ABurrowerEnemy>(BurrowerController->GetPawn());
		if (Burrower)
		{
			// Check if burrower is tutorial burrower
			BBComp->SetValueAsBool(IsTutorialKey.SelectedKeyName, Burrower->bIsTutorialBurrower);
		}
	}
	
}

/*
UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if ((uint8)BBComp->GetValueAsEnum(StateKey.SelectedKeyName) == (uint8)EBurrowActionState::Patrolling)
	{
		AGameStateBase* GameState = UGameplayStatics::GetGameState(World);
	
		for (APlayerState* PlayerState : GameState->PlayerArray)
		{
			APawn* Character = PlayerState->GetPawn();
	
			UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);
			FNavLocation NavLoc;
			ANavigationData* UseNavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
			FSharedConstNavQueryFilter NavQueryFilter = UNavigationQueryFilter::GetQueryFilter(*UseNavData, QueryFilter);
			if (NavSys->TestPathSync(FVector::ZeroVector, 500, NavLoc, UseNavData, NavQueryFilter))
			{
				
			}
		}
	}
 */
