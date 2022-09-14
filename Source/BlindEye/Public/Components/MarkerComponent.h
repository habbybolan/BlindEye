// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarkerComponent.generated.h"

enum class PlayerType : uint8;

UCLASS()
class BLINDEYE_API UMarkerComponent : public USceneComponent
{
	GENERATED_BODY()
	
public:	
	UMarkerComponent();

	void SetPlayerMarkMesh(PlayerType PlayerMarkToSet); 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* CrowMark;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* PhoenixMark; 

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void PlaySpawnEffect();
	void RemoveMark();
};
