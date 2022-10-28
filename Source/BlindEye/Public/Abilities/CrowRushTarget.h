// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrowRushTarget.generated.h"

UCLASS()
class BLINDEYE_API ACrowRushTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACrowRushTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
