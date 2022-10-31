// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "BaseIsland.h"
#include "ShrineIsland.generated.h"

UCLASS()
class BLINDEYE_API AShrineIsland : public ABaseIsland
{
	GENERATED_BODY()
	
public:	
	AShrineIsland();

protected:
	virtual void BeginPlay() override;

};
