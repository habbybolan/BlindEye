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

	// UWorld* World = GetWorld();
	// if (World == nullptr) return;
	//
	// UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	// if ((uint8)BBComp->GetValueAsEnum(StateKey.SelectedKeyName) == (uint8)EBurrowActionState::Patrolling)
	// {
	// 	AGameStateBase* GameState = UGameplayStatics::GetGameState(World);
	//
	// 	for (APlayerState* PlayerState : GameState->PlayerArray)
	// 	{
	// 		APawn* Character = PlayerState->GetPawn();
	//
	// 		UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);
	// 		FNavLocation NavLoc;
	// 		ANavigationData* UseNavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
	// 		FSharedConstNavQueryFilter NavQueryFilter = UNavigationQueryFilter::GetQueryFilter(*UseNavData, QueryFilter);
	// 		if (NavSys->TestPathSync(FVector::ZeroVector, 500, NavLoc, UseNavData, NavQueryFilter))
	// 		{
	// 			
	// 		}
	// 	}
	// }
}
