// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "MarkerComponent.generated.h"

enum class PlayerType : uint8;

UCLASS()
class BLINDEYE_API UMarkerComponent : public USceneComponent
{
	GENERATED_BODY()
	
public:	
	UMarkerComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AStaticMeshActor> CrowMarkType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AStaticMeshActor> PhoenixMarkType;

	UFUNCTION(NetMulticast, Reliable)
	void MULT_RemoveMark();
	UFUNCTION(NetMulticast, Reliable)
	void MULT_DetonateMark();
	UFUNCTION(NetMulticast, Reliable) 
	void MULT_AddMark(PlayerType PlayerMarkToSet); 

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	UPROPERTY(Replicated)
	AStaticMeshActor* CrowMark;
	UPROPERTY(Replicated)
	AStaticMeshActor* PhoenixMark;

	bool bMarked = false;
};
