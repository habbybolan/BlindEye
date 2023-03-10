// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Snapper/SnapperEnemyController.h"

#include "Shrine.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Enemies/Snapper/SnapperEnemy.h"
#include "Kismet/GameplayStatics.h"

void ASnapperEnemyController::BeginPlay()
{
	Super::BeginPlay();

	InitializeBehaviorTree();
	SetupShrineReference();
}

void ASnapperEnemyController::SetTargetEnemy(AActor* target)
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp == nullptr) return;
	
	GetBlackboardComponent()->SetValueAsObject(TEXT("EnemyActor"), target);
}

bool ASnapperEnemyController::CanJumpAttack(AActor* target)
{ 
	return !Snapper->IsJumpAttackOnDelay && IsInJumpAttackRange(target);
}

bool ASnapperEnemyController::IsInJumpAttackRange(AActor* Target) 
{
	if (Target != nullptr)
	{
		FVector TargetLocation = Target->GetActorLocation();

		if (!Snapper) return false;
		return FVector::Distance(TargetLocation, Snapper->GetActorLocation()) < Snapper->DistanceToJumpAttack;
	}
	return false;
}

void ASnapperEnemyController::PerformJumpAttack()
{
	Snapper->PerformJumpAttack();
}

bool ASnapperEnemyController::CanBasicAttack(AActor* target)
{
	return !Snapper->IsBasicAttackOnDelay &&
			IsInBasicAttackRange(target) &&
			Snapper->GetShrineAttackPoint() != nullptr;
}

bool ASnapperEnemyController::IsInBasicAttackRange(AActor* Target)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return false;

	if (!Snapper) return false;

	FHitResult OutHit;
	if (UKismetSystemLibrary::LineTraceSingleForObjects(World, Snapper->GetActorLocation(), Snapper->GetActorLocation() + Snapper->GetActorForwardVector() * 300,
		ObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::None, OutHit, true))
	{
		return OutHit.Actor == Target;
	}
	return false;
}

void ASnapperEnemyController::DamageTaken(float Damage, FVector HitLocation, const UDamageType* DamageType,
	AActor* DamageCauser)
{
	if (ABlindEyePlayerCharacter* BlindEyePlayer = Cast<ABlindEyePlayerCharacter>(DamageCauser))
	{
		AActor* BBTarget = GetBTTarget();
		if (!Snapper) return;

		// if targeting shrine or no target selected
		if (Snapper->IsAttackingShrine == true && BBTarget == nullptr)
		{
			SetTargetEnemy(DamageCauser);
			Snapper->IsAttackingShrine = false;
		}
		// Other player attack snapper while 
		else { return; }
		
		GetWorldTimerManager().SetTimer(CooldownToAttackShrineTimerHandle,
			this, &ASnapperEnemyController::SetupShrineReference, DelayUntilAttackShrineAgain, false);
	}
}

void ASnapperEnemyController::OnSnapperDeath()
{
	GetBrainComponent()->StopLogic("Death");
}

void ASnapperEnemyController::PerformBasicAttack()
{
	if (Snapper)
	{
		Snapper->PerformBasicAttack();
	}
}

void ASnapperEnemyController::SetupShrineReference()
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	AActor* ShrineActor = UGameplayStatics::GetActorOfClass(world, AShrine::StaticClass());
	
	if (ShrineActor)
	{
		UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
		if (BlackboardComp == nullptr) return;
	 
		GetBlackboardComponent()->SetValueAsObject(TEXT("ShrineActor"), ShrineActor);
	}
}

void ASnapperEnemyController::SetAttackingShrine()
{
	if (Snapper)
	{
		Snapper->IsAttackingShrine = true;
	}
}

void ASnapperEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	Snapper = Cast<ASnapperEnemy>(InPawn);
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
