// Copyright (C) Nicholas Johnson 2022


#include "Enemies/EQC_PlayerContext.h"

#include "Enemies/BlindEyeEnemybase.h"
#include "Enemies/BlindEyeEnemyController.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEQC_PlayerContext::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	ABlindEyeEnemyBase* QueryOwner = Cast<ABlindEyeEnemyBase>(QueryInstance.Owner.Get());
	ABlindEyeEnemyController* EnemyController = Cast<ABlindEyeEnemyController>(QueryOwner->GetController());
	AActor* Target = EnemyController->GetBTTarget();
	if (Target == nullptr) return;
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, Target);
}
