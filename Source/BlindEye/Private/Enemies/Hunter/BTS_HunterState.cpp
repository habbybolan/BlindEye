// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/BTS_HunterState.h"

#include "AIController.h"
#include "Shrine.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Hunter/HunterEnemyController.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

UBTS_HunterState::UBTS_HunterState()
{
	NodeName = TEXT("Hunter State");
	bNotifyTick = true;
}

void UBTS_HunterState::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	UBlackboardComponent* BBComp = SearchData.OwnerComp.GetBlackboardComponent();
	check(BBComp);

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	// check if game ended
	ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
	if (BlindEyeGS->IsBlindEyeMatchEnding() || BlindEyeGS->IsBlindEyeMatchEnded())
	{
		BBComp->SetValueAsBool(IsGameEndedKey.SelectedKeyName, true);
	}
	
	if (BBComp->GetValueAsBool(IsFirstRunKey.SelectedKeyName))
	{
		AAIController* Controller = SearchData.OwnerComp.GetAIOwner();
		HunterController = Cast<AHunterEnemyController>(Controller);
		Hunter = Cast<AHunterEnemy>(HunterController->GetPawn());

		if (AActor* Actor = UGameplayStatics::GetActorOfClass(World, AShrine::StaticClass()))
		{
			AShrine* Shrine = Cast<AShrine>(Actor);
			BBComp->SetValueAsObject(ShrineKey.SelectedKeyName, Shrine);
		}
		BBComp->SetValueAsBool(IsDeadKey.SelectedKeyName, false);
		BBComp->SetValueAsBool(IsFleeingKey.SelectedKeyName, false);
	}
}

void UBTS_HunterState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BBComp))
	{
		BBComp->SetValueAsEnum(IsAttackingKey.SelectedKeyName, (uint8)Hunter->GetCurrAttack());
		BBComp->SetValueAsBool(bChargedOnCooldownKey.SelectedKeyName, Hunter->GetIsCharged());
		BBComp->SetValueAsBool(IsChannellingKey.SelectedKeyName, Hunter->GetIsChannelling());
		BBComp->SetValueAsBool(IsFleeingKey.SelectedKeyName, Hunter->GetIsFleeing());
	}
}
