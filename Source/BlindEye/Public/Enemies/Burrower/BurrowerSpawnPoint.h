// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "BurrowerSpawnPoint.generated.h"

UCLASS()
class BLINDEYE_API ABurrowerSpawnPoint : public AActor
{
	GENERATED_BODY()

	//UArrowComponent* ArrowComponent;

	
public:	
	// Sets default values for this actor's properties
	ABurrowerSpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
