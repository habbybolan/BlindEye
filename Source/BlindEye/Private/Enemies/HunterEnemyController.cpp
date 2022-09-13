// Copyright (C) Nicholas Johnson 2022


#include "Enemies/HunterEnemyController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/BurrowerSpawnPoint.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void AHunterEnemyController::BeginPlay()
{
	Super::BeginPlay();
	SpawnOnDelay(nullptr);
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
	
	UWorld* world = GetWorld();
	if (!world) return;
	
	Hunter = Cast<AHunterEnemy>(InPawn);
	
	if (const IHealthInterface* HealthInterface = Cast<IHealthInterface>(Hunter))
	{
		HealthInterface->Execute_GetHealthComponent(Hunter)->OnDeathDelegate.AddUFunction(this, FName("SpawnOnDelay"));
	}

	// Get random player to attack
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
	InitializeBehaviorTree();
	SetTargetEnemy(RandPlayerTarget);
}

void AHunterEnemyController::SetCanBasicAttack()
{
	IsBasicAttackOnDelay = false;
}

void AHunterEnemyController::SpawnOnDelay(AActor* HunterKilled)
{
	UWorld* world = GetWorld();
	if (!world) return;

	world->GetTimerManager().SetTimer(SpawnDelayTimerHandle, this, &AHunterEnemyController::SpawnHunter, SpawnDelay, false);
}

void AHunterEnemyController::SpawnHunter() 
{
	UWorld* world = GetWorld();
	if (!world) return;

	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(world, ABurrowerSpawnPoint::StaticClass(),SpawnPoints);
	if (SpawnPoints.Num() == 0) return;
	AActor* SpawnPoint = SpawnPoints[UKismetMathLibrary::RandomIntegerInRange(0, SpawnPoints.Num() - 1)];
	
	FVector SpawnLocation = SpawnPoint->GetActorLocation();
	FRotator Rotation = SpawnPoint->GetActorRotation();
	FActorSpawnParameters params; 
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AHunterEnemy* SpawnedHunter = world->SpawnActor<AHunterEnemy>(HunterType, SpawnLocation, Rotation, params);
	Possess(SpawnedHunter);
}
