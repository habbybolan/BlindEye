// Copyright (C) Nicholas Johnson 2022


#include "Enemies/BlindEyeEnemyController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Components/HealthComponent.h"
#include "Interfaces/HealthInterface.h"

AActor* ABlindEyeEnemyController::GetBTTarget()
{
	UBlackboardComponent* BBComp = GetBlackboardComponent();
	if (BBComp == nullptr) return nullptr;

	UObject* ObjectActor = BBComp->GetValueAsObject("EnemyActor");
	if (ObjectActor == nullptr) return nullptr;

	return Cast<AActor>(ObjectActor);
}

void ABlindEyeEnemyController::BeginPlay()
{
	Super::BeginPlay();
}

void ABlindEyeEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(InPawn))
	{ 
		UHealthComponent* HealthComponent = HealthInterface->GetHealthComponent(); 
		if (!HealthComponent) return;
		HealthComponent->StunStartDelegate.AddUFunction(this, TEXT("OnStunStart"));
		HealthComponent->StunEndDelegate.AddUFunction(this, TEXT("OnStunEnd"));
		HealthComponent->BurnDelegateStart.AddUFunction(this, TEXT("OnBurnStart"));
		HealthComponent->BurnDelegateEnd.AddUFunction(this, TEXT("OnBurnEnd"));
		HealthComponent->TauntStartDelegate.AddUFunction(this, TEXT("OnTauntStart"));
		HealthComponent->TauntEndDelegate.AddUFunction(this, TEXT("OnTauntEnd"));
	}
}

void ABlindEyeEnemyController::OnStunStart(float StunDuration)
{
}

void ABlindEyeEnemyController::OnStunEnd()
{
}

void ABlindEyeEnemyController::OnBurnStart(float DamagePerSec, float Duration)
{
}

void ABlindEyeEnemyController::OnBurnEnd()
{
}

void ABlindEyeEnemyController::OnTauntStart(float Duration, AActor* Taunter)
{
}

void ABlindEyeEnemyController::OnTauntEnd()
{
}
