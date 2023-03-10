// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Snapper/BTS_SnapperState.h"

#include "AIController.h"
#include "Shrine.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Snapper/SnapperEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"

void UBTS_SnapperState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!BBComp) return;

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	// check if game ended
	ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
	if (BlindEyeGS->IsBlindEyeMatchEnding() || BlindEyeGS->IsBlindEyeMatchEnded())
	{
		BBComp->SetValueAsBool(IsGameEndedKey.SelectedKeyName, true);
	}
	
	if (AAIController* Controller = OwnerComp.GetAIOwner())
	{
		if (Controller->GetPawn())
		{
			ASnapperEnemy* Snapper = Cast<ASnapperEnemy>(Controller->GetPawn());

			BBComp->SetValueAsBool(IsRagdolling.SelectedKeyName, Snapper->GetIsRagdolling());
			BBComp->SetValueAsEnum(AttackState.SelectedKeyName, (uint8)Snapper->CurrAttack);
			BBComp->SetValueAsBool(IsAttackingShrine.SelectedKeyName, Snapper->IsAttackingShrine);

			BBComp->SetValueAsObject(ShrineAttackObjectKey.SelectedKeyName, Snapper->GetShrineAttackPoint());
			if (UObject* AttackPointObj = BBComp->GetValueAsObject(ShrineAttackObjectKey.SelectedKeyName))
			{
				UShrineAttackPoint* ShrineAttackObj = Cast<UShrineAttackPoint>(AttackPointObj);
				BBComp->SetValueAsVector(ShrineAttackPositionKey.SelectedKeyName, ShrineAttackObj->Location);
				Snapper->GetCharacterMovement()->MaxWalkSpeed = Snapper->AttackingShrineMoveSpeed;
			} else
			{
				Snapper->GetCharacterMovement()->MaxWalkSpeed = Snapper->BaseMoveSpeed;
			}
		}
	}
}
