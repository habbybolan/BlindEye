// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BTS_BurrowerState.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Burrower/BurrowerEnemyController.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UBTS_BurrowerState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AAIController* Controller = OwnerComp.GetAIOwner();
	ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller);

	// Check if attack state completely finished
	if (BurrowerController->IsHidden() &&
		BBComp->GetValueAsEnum(StateKey.SelectedKeyName) == (uint8)EBurrowActionState::Attacking &&
		BBComp->GetValueAsObject(EnemyActorKey.SelectedKeyName) == nullptr)
	{
		BBComp->SetValueAsEnum(StateKey.SelectedKeyName, (uint8)EBurrowActionState::Patrolling);
	}

	EBurrowActionState CurrState = (EBurrowActionState)BBComp->GetValueAsEnum(StateKey.SelectedKeyName);

	// Check if Spawn time should be activated / is queued up
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
		if (CurrState != EBurrowActionState::Spawning)
		{
			// If Enemy actor set
			if (BBComp->GetValueAsObject(EnemyActorKey.SelectedKeyName))
			{
				BBComp->SetValueAsEnum(StateKey.SelectedKeyName, (uint8)EBurrowActionState::Attacking);
			}
			// Otherwise no player reference, so patrol
			else
			{
				BBComp->SetValueAsEnum(StateKey.SelectedKeyName, (uint8)EBurrowActionState::Patrolling);
			}
		}
	}
	
	// Check if time to spawn
	if (CurrState != EBurrowActionState::Spawning)
	{
		CurrSpawnStateTime += DeltaSeconds;
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
