// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Snapper/SnapperEnemyController.h"

#include "Shrine.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Snapper/SnapperEnemy.h"
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

bool ASnapperEnemyController::CanJumpAttack(AActor* target)
{ 
	return !IsJumpAttackOnDelay && IsInJumpAttackRange(target);
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
	Snapper->PerformJumpAttack();
	IsJumpAttackOnDelay = true;
	GetWorldTimerManager().SetTimer(JumpAttackDelayTimerHandle, this, &ASnapperEnemyController::SetCanJumpAttack, JumpAttackDelay, false);
}

bool ASnapperEnemyController::CanBasicAttack(AActor* target)
{
	return !IsBasicAttackOnDelay && IsInBasicAttackRange(target);
}

bool ASnapperEnemyController::IsInBasicAttackRange(AActor* Target)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return false;

	if (!Snapper) return false;

	FHitResult OutHit;
	if (UKismetSystemLibrary::LineTraceSingleForObjects(World, Snapper->GetActorLocation(), Snapper->GetActorLocation() + Snapper->GetActorForwardVector() * 300,
		ObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, OutHit, true))
	{
		return OutHit.Actor == Target;
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

void ASnapperEnemyController::SetCanJumpAttack()
{
	IsJumpAttackOnDelay = false;
}

void ASnapperEnemyController::SetCanBasicAttack()
{
	IsBasicAttackOnDelay = false;
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
