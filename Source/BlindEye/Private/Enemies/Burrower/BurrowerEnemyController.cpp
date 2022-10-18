// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BurrowerEnemyController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Enemies/Burrower/BurrowerEnemy.h"
#include "Enemies/Burrower/BurrowerSpawnPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

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
	return bSurfacing;
}

bool ABurrowerEnemyController::IsHiding()
{
	return bHiding;
}

void ABurrowerEnemyController::CacheSpawnPoints()
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	UGameplayStatics::GetAllActorsOfClass(world, ABurrowerSpawnPoint::StaticClass(), SpawnLocation);
}

FTransform ABurrowerEnemyController::FindRandSpawnPoint()
{
	// TODO: remove and move to separate task
	if (SpawnLocation.Num() == 0) return FTransform();
	uint32 randIndex = UKismetMathLibrary::RandomInteger(SpawnLocation.Num());
	return SpawnLocation[randIndex]->GetTransform();
}

void ABurrowerEnemyController::StopWarningParticles()
{
	CachedBurrower = Cast<ABurrowerEnemy>(GetPawn());
	if (!CachedBurrower) return;

	CachedBurrower->MULT_DespawnWarningParticle();
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
	
	TArray<ABlindEyePlayerCharacter*> PlayersOnIsland = CachedBurrower->Listener->GetPlayersOnIsland(CachedBurrower->IslandType);
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

TArray<ABlindEyePlayerCharacter*> ABurrowerEnemyController::GetPlayersOnIsland()
{
	return CachedBurrower->Listener->GetPlayersOnIsland(CachedBurrower->IslandType);
}

void ABurrowerEnemyController::StartWarningParticles()
{
	CachedBurrower = Cast<ABurrowerEnemy>(GetPawn());
	if (!CachedBurrower) return;

	CachedBurrower->MULT_SpawnWarningParticle();
}

void ABurrowerEnemyController::AddNewActionState(EBurrowActionState NewAction)
{
	for (int i = CachedPreviousActions.Num() - 1; i > 0; i--)
	{
		CachedPreviousActions[i] = CachedPreviousActions[i - 1];
	}
	CachedPreviousActions[0] = NewAction;
}

void ABurrowerEnemyController::ActionStateFinished()
{
	// TODO: Notify Behavior tree action state finished
}

void ABurrowerEnemyController::SurfacingFinished()
{
	bSurfacing = false;
}

void ABurrowerEnemyController::HidingFinished()
{
	bHiding = false;
}

void ABurrowerEnemyController::StartSurfacing()
{
	CachedBurrower = Cast<ABurrowerEnemy>(GetPawn());
	if (!CachedBurrower) return;
	
	bSurfacing = true;
	CachedBurrower->StartSurfacing();
}

void ABurrowerEnemyController::StartHiding()
{
	CachedBurrower = Cast<ABurrowerEnemy>(GetPawn());
	if (!CachedBurrower) return;
	
	bHiding = true;
	CachedBurrower->StartHiding();
}
 
void ABurrowerEnemyController::SetBurrowerState(bool isHidden, bool bFollowing)
{
	CachedBurrower = Cast<ABurrowerEnemy>(GetPawn());
	if (!CachedBurrower) return;

	CachedBurrower->MULT_SetBurrowerState(isHidden, bFollowing);
}

void ABurrowerEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	CachedBurrower = Cast<ABurrowerEnemy>(GetPawn());
	if (!CachedBurrower) return;

	CachedBurrower->ActionStateFinished.BindUFunction(this, FName("ActionStateFinished"));
	CachedBurrower->SurfacingFinished.BindUFunction(this, FName("SurfacingFinished"));
	CachedBurrower->HidingFinished.BindUFunction(this, FName("HidingFinished"));
}
