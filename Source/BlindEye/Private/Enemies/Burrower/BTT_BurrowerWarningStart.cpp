// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BTT_BurrowerWarningStart.h"

#include "AIController.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

UBTT_BurrowerWarningStart::UBTT_BurrowerWarningStart()
{
	NodeName = TEXT("Warning");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_BurrowerWarningStart::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UWorld* world = GetWorld();
	if (world == nullptr) return EBTNodeResult::Failed;

	FBTBurrowerWaitTaskMemory* MyMemory = (FBTBurrowerWaitTaskMemory*)NodeMemory;
	MyMemory->RemainingWaitTime = WarningDelay;
	
	MULT_SpawnWarningParticle(&OwnerComp);
	return EBTNodeResult::InProgress;
}

void UBTT_BurrowerWarningStart::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTBurrowerWaitTaskMemory* MyMemory = (FBTBurrowerWaitTaskMemory*)NodeMemory;
	MyMemory->RemainingWaitTime -= DeltaSeconds;

	if (MyMemory->RemainingWaitTime <= 0.0f)
	{
		// continue execution from this node
		MyMemory->RemainingWaitTime = 0.0f;
		EndWarning(OwnerComp);
	}
}

uint16 UBTT_BurrowerWarningStart::GetInstanceMemorySize() const
{
	return sizeof(FBTBurrowerWaitTaskMemory);
}

void UBTT_BurrowerWarningStart::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);

	FBTBurrowerWaitTaskMemory* MyMemory = (FBTBurrowerWaitTaskMemory*)NodeMemory;
	if (MyMemory->RemainingWaitTime)
	{
		Values.Add(FString::Printf(TEXT("remaining: %ss"), *FString::SanitizeFloat(MyMemory->RemainingWaitTime)));
	}
}

void UBTT_BurrowerWarningStart::EndWarning(UBehaviorTreeComponent& OwnerComp)
{
	MULT_DespawnWarningParticle();
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

void UBTT_BurrowerWarningStart::MULT_SpawnWarningParticle_Implementation(UBehaviorTreeComponent* OwnerComp)
{
	UWorld* world = GetWorld();
	if (world == nullptr) return;

	AAIController* Controller = OwnerComp->GetAIOwner();
	if (APawn* Pawn = Controller->GetPawn())
	{
		if (ACharacter* Character = Cast<ACharacter>(Pawn))
		{
			SpawnedWarningParticle = UNiagaraFunctionLibrary::SpawnSystemAtLocation(world, WarningParticle,
			Character->GetActorLocation() + FVector::UpVector * (Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2),
			FRotator::ZeroRotator, FVector::OneVector, true);
		}
	}
}

void UBTT_BurrowerWarningStart::MULT_DespawnWarningParticle_Implementation()
{
	if (SpawnedWarningParticle)
	{
		SpawnedWarningParticle->Deactivate();
	}
}
