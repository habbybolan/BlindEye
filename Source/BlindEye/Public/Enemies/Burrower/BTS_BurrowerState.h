// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_BurrowerState.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UBTS_BurrowerState : public UBTService
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=BlackBoard)
	FBlackboardKeySelector StateKey;

	UPROPERTY(EditAnywhere, Category=BlackBoard) 
	FBlackboardKeySelector EnemyActorKey;

	UPROPERTY(EditAnywhere, Category=BlackBoard) 
	FBlackboardKeySelector VisibilityStateKey;

	UPROPERTY(EditAnywhere, Category=BlackBoard)
	FBlackboardKeySelector IsTutorialKey;

	UPROPERTY(EditAnywhere)
	float SpawnStateDelay = 15.f;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData) override;

	float CurrSpawnStateTime = 0;
	bool bSpawnStateQueued = false;
	
	
};
