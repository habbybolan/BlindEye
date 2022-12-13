// Copyright (C) Nicholas Johnson 2022


#include "Enemies/BTT_PlayAnimationReplication.h"

#include "AIController.h"
#include "Enemies/BlindEyeEnemyBase.h"

EBTNodeResult::Type UBTT_PlayAnimationReplication::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = EBTNodeResult::Failed;
	AAIController* Controller = OwnerComp.GetAIOwner();
	MyOwnerComp = &OwnerComp;
	if (Controller && Controller->GetPawn())
	{
		ABlindEyeEnemyBase* Enemy = Cast<ABlindEyeEnemyBase>(Controller->GetPawn());
		check(Enemy)
 
		Enemy->MULT_PlayAnimMontage(AnimToPlay);
		Enemy->GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &UBTT_PlayAnimationReplication::OnAnimEnded);

		Result = EBTNodeResult::InProgress;
	}
	return Result; 
}

EBTNodeResult::Type UBTT_PlayAnimationReplication::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CleanUp(OwnerComp);

	return EBTNodeResult::Aborted;
}

void UBTT_PlayAnimationReplication::OnAnimEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (MyOwnerComp && AnimToPlay == Montage)
	{
		FinishLatentTask(*MyOwnerComp, EBTNodeResult::Succeeded);
		CleanUp(*MyOwnerComp);
	}
}

void UBTT_PlayAnimationReplication::CleanUp(UBehaviorTreeComponent& OwnerComp)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (Controller && Controller->GetPawn())
	{
		if (ABlindEyeEnemyBase* Enemy = Cast<ABlindEyeEnemyBase>(Controller->GetPawn()))
		{
			Enemy->GetMesh()->GetAnimInstance()->OnMontageEnded.Remove(this, "OnAnimEnded");
		}
	}
}
