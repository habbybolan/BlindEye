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
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Blue, "Enemy Stunned: " + FString::SanitizeFloat(StunDuration));
}

void ABlindEyeEnemyController::OnStunEnd()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Blue, "Stun ended");
}

void ABlindEyeEnemyController::OnBurnStart(float DamagePerSec, float Duration)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Blue, "Burn Started");
}

void ABlindEyeEnemyController::OnBurnEnd()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Blue, "Burn ended");
}

void ABlindEyeEnemyController::OnTauntStart(float Duration, AActor* Taunter)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Blue, "Taunt started for " + FString::SanitizeFloat(Duration) + "from: " + Taunter->GetName());
}

void ABlindEyeEnemyController::OnTauntEnd()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Blue, "Taunt ended");
}
