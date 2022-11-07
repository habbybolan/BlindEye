// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BurrowerEnemyController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Enemies/Burrower/BurrowerEnemy.h"
#include "Enemies/Burrower/BurrowerSpawnPoint.h"
#include "Kismet/GameplayStatics.h"

void ABurrowerEnemyController::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();
	if (!world) return;

	CachedPreviousActions.SetNum(3, false);
	CachedPreviousActions[0] = EBurrowActionState::Attacking;

	InitializeBehaviorTree();
	CacheSpawnPoints();
}

void ABurrowerEnemyController::SpawnSnappers()
{
	CachedBurrower = Cast<ABurrowerEnemy>(GetPawn());
	if (!CachedBurrower) return;

	CachedBurrower->SpawnSnappers();
}

EBurrowActionState ABurrowerEnemyController::GetCurrAction()
{
	return CachedPreviousActions[0];
}

void ABurrowerEnemyController::CalcNewActionState()
{
	if (CachedPreviousActions[0] == EBurrowActionState::Spawning)
	{
		AddNewActionState(EBurrowActionState::Attacking);
	} else
	{
		AddNewActionState(EBurrowActionState::Spawning);
	}
}

bool ABurrowerEnemyController::IsSurfacing()
{
	if (CachedBurrower == nullptr) return false;
	
	return CachedBurrower->GetIsSurfacing();
}

bool ABurrowerEnemyController::IsSurfaced()
{
	if (CachedBurrower == nullptr) return false;
	
	return CachedBurrower->GetIsSurfaced();
}

bool ABurrowerEnemyController::IsHidden()
{
	if (CachedBurrower == nullptr) return false;
	
	return CachedBurrower->GetIsHidden();
}

bool ABurrowerEnemyController::IsHiding()
{
	if (CachedBurrower == nullptr) return false;
	
	return CachedBurrower->GetIsHiding();
}

void ABurrowerEnemyController::CacheSpawnPoints()
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	UGameplayStatics::GetAllActorsOfClass(world, UBurrowerSpawnPoint::StaticClass(), SpawnLocation);
}

void ABurrowerEnemyController::StopWarningParticles()
{
	CachedBurrower = Cast<ABurrowerEnemy>(GetPawn());
	if (!CachedBurrower) return;

	CachedBurrower->WarningEnded();
}

void ABurrowerEnemyController::NotifyPlayerEnteredIsland(ABlindEyePlayerCharacter* Player)
{
	UBrainComponent* Brain = GetBrainComponent();
	if (Brain == nullptr) return;

	UBlackboardComponent* BBComp = Brain->GetBlackboardComponent();
	UObject* EnemyObject = BBComp->GetValueAsObject(TEXT("EnemyActor"));
	if (EnemyObject == nullptr)
	{
		BBComp->SetValueAsObject(TEXT("EnemyActor"), Player);
	}
}

void ABurrowerEnemyController::NotifyPlayerLeftIsland(ABlindEyePlayerCharacter* Player)
{
	// TODO: Check if Player is equal to EnemyActor
	//		If so, then check if any other player on the island
	//		Otherwise, ClearValue normally

	UBrainComponent* Brain = GetBrainComponent();
	if (Brain == nullptr) return;
	UBlackboardComponent* BBComp = Brain->GetBlackboardComponent();
	
	TArray<ABlindEyePlayerCharacter*> PlayersOnIsland = CachedBurrower->Listener->GetPlayersOnIsland(CachedBurrower->IslandID);
	// If players still on island, set to attack remaining player
	if (PlayersOnIsland.Num() > 0)
	{
		BBComp->SetValueAsObject(TEXT("EnemyActor"), PlayersOnIsland[0]);
	}
	// Otherwise, go back to patrolling state
	else
	{
		BBComp->ClearValue(TEXT("EnemyActor"));
	}
}

void ABurrowerEnemyController::CancelHide()
{
	if (CachedBurrower != nullptr)
	{
		CachedBurrower->CancelHide();
	}
}

TArray<ABlindEyePlayerCharacter*> ABurrowerEnemyController::GetPlayersOnIsland()
{
	return CachedBurrower->Listener->GetPlayersOnIsland(CachedBurrower->IslandID);
}

void ABurrowerEnemyController::SetInBurstState()
{
	check(Blackboard)
	Blackboard->SetValueAsEnum("ActionState", (uint8)EBurrowActionState::BurstWave);
}

void ABurrowerEnemyController::NotifySpawningStopped()
{
	CachedBurrower->NotifySpawningStopped();
}

void ABurrowerEnemyController::StartWarningParticles()
{
	CachedBurrower = Cast<ABurrowerEnemy>(GetPawn());
	if (!CachedBurrower) return;

	CachedBurrower->WarningStarted();
}

void ABurrowerEnemyController::AddNewActionState(EBurrowActionState NewAction)
{
	for (int i = CachedPreviousActions.Num() - 1; i > 0; i--)
	{
		CachedPreviousActions[i] = CachedPreviousActions[i - 1];
	}
	CachedPreviousActions[0] = NewAction;
}

void ABurrowerEnemyController::StartSurfacing()
{
	CachedBurrower = Cast<ABurrowerEnemy>(GetPawn());
	if (!CachedBurrower) return;
	
	CachedBurrower->StartSurfacing();
}

void ABurrowerEnemyController::StartHiding()
{
	CachedBurrower = Cast<ABurrowerEnemy>(GetPawn());
	if (!CachedBurrower) return;
	
	CachedBurrower->StartHiding();
}
 
// void ABurrowerEnemyController::SetBurrowerState(bool isHidden, bool bFollowing)
// {
// 	CachedBurrower = Cast<ABurrowerEnemy>(GetPawn());
// 	if (!CachedBurrower) return;
//
// 	CachedBurrower->MULT_SetBurrowerState(isHidden);
// }

void ABurrowerEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	CachedBurrower = Cast<ABurrowerEnemy>(GetPawn());
	if (!CachedBurrower) return;

	UActorComponent* Component = CachedBurrower->GetComponentByClass(UHealthComponent::StaticClass());
	check(Component);
	UHealthComponent* HealthComponent = Cast<UHealthComponent>(Component);
	HealthComponent->DetonateDelegate.AddDynamic(this, &ABurrowerEnemyController::OnDetonated);
}

void ABurrowerEnemyController::OnDetonated()
{
	// On detonation, set state as cancelled
	UBlackboardComponent* BBComp = GetBlackboardComponent();
	if (BBComp)
	{
		BBComp->SetValueAsEnum("ActionState", (uint8)EBurrowActionState::Cancelled);
	}
}
