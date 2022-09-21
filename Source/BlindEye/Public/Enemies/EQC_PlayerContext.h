// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EQC_PlayerContext.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UEQC_PlayerContext : public UEnvQueryContext
{
	GENERATED_BODY()

	
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
