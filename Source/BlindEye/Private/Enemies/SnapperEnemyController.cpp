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
}

bool ASnapperEnemyController::CanJumpAttack()
{
	return !IsJumpAttackOnDelay;
}

bool ASnapperEnemyController::IsInJumpAttackRange(AActor* Target) 
{
	if (Target != nullptr)
	{
		FVector TargetLocation = Target->GetActorLocation();

		if (!Snapper) return false;
		return FVector::Distance(TargetLocation, Snapper->GetActorLocation()) < DistanceToJumpAttack;
	}
	return false;
}

void ASnapperEnemyController::PerformJumpAttack()
{
	if (!CanJumpAttack()) return;
	
	Snapper->PerformJumpAttack();
	IsJumpAttackOnDelay = true;
	GetWorldTimerManager().SetTimer(JumpAttackDelayTimerHandle, this, &ASnapperEnemyController::SetCanBasicAttack, JumpAttackDelay, false);
}

void ASnapperEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	Snapper = Cast<ASnapperEnemy>(InPawn);
}

void ASnapperEnemyController::SetCanBasicAttack()
{
	IsJumpAttackOnDelay = false;
}

void ASnapperEnemyController::OnTauntStart(float Duration, AActor* Taunter)
{
	Super::OnTauntStart(Duration, Taunter);
	
	if (GetBlackboardComponent() == nullptr) return;
	GetBlackboardComponent()->SetValueAsObject(TEXT("TauntActor"), Taunter);
}

void ASnapperEnemyController::OnTauntEnd()
{
	Super::OnTauntEnd();
	
	if (GetBlackboardComponent() == nullptr) return;
	GetBlackboardComponent()->SetValueAsObject(TEXT("TauntActor"), nullptr);
}

void ASnapperEnemyController::OnStunStart(float StunDuration)
{
	Super::OnStunStart(StunDuration);
	// TODO: Play stun animation
}

void ASnapperEnemyController::OnStunEnd()
{
	Super::OnStunEnd();
	// TODO: Stop stun animation, return control to BB
}
