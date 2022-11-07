// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BTT_BurrowerEndSpawning.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/Burrower/BurrowerEnemyController.h"

EBTNodeResult::Type UBTT_BurrowerEndSpawning::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();

	AAIController* Controller = OwnerComp.GetAIOwner();
	if (ABurrowerEnemyController* BurrowerController = Cast<ABurrowerEnemyController>(Controller))
	{
		APawn* Burrower = BurrowerController->GetPawn();
		check(Burrower);
		
		TArray<ABlindEyePlayerCharacter*> PlayersOnIsland = BurrowerController->GetPlayersOnIsland();
		// if players on island exist, find closest player to burrower
		if (PlayersOnIsland.Num() > 0)
		{
			ABlindEyePlayerCharacter* ClosestPlayer = PlayersOnIsland[0];
			float ClosestDist = FVector::Distance(ClosestPlayer->GetActorLocation(), Burrower->GetActorLocation());

			for (uint8 i = 1; i < PlayersOnIsland.Num(); i++)
			{
				ABlindEyePlayerCharacter* CheckPlayer = PlayersOnIsland[i];
				float DistCheck = FVector::Distance(CheckPlayer->GetActorLocation(), Burrower->GetActorLocation());
				if (DistCheck < ClosestDist)
				{
					ClosestPlayer = CheckPlayer;
				}
			}

			BBComp->SetValueAsObject(EnemyActorKey.SelectedKeyName, ClosestPlayer);
			BBComp->SetValueAsEnum(StateKey.SelectedKeyName, (uint8)EBurrowActionState::Attacking);
		}
		// Otherwise no player island, goto patrol state
		else
		{
			BBComp->SetValueAsEnum(StateKey.SelectedKeyName, (uint8)EBurrowActionState::Patrolling);
		}
		BurrowerController->NotifySpawningStopped();
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
