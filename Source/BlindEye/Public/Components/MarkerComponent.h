// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "MarkerComponent.generated.h"

enum class EPlayerType : uint8;

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

	void RemoveMark();
	void DetonateMark();
	void AddMark(EPlayerType PlayerMarkToSet); 

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	float CachedZPosition;

	UFUNCTION()
	virtual void OnOwnerDestroyed(AActor* OwnerDestroyed);

	UPROPERTY()
	AStaticMeshActor* CrowMark;
	UPROPERTY()
	AStaticMeshActor* PhoenixMark;

	bool bMarked = false;
};
