// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarkerComponent.generated.h"

enum class EPlayerType : uint8;
class AMarkerStaticMesh;

UENUM(BlueprintType)
enum class EMarkerType : uint8
{
	Crow,
	Phoenix,
	Hunter
};

UCLASS()
class BLINDEYE_API UMarkerComponent : public USceneComponent
{
	GENERATED_BODY()
	
public:	
	UMarkerComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AMarkerStaticMesh> CrowMarkType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AMarkerStaticMesh> PhoenixMarkType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AMarkerStaticMesh> HunterMarkType;
 
	void RemoveMark();
	void DetonateMark(EMarkerType MarkerType);
	void AddMark(EMarkerType PlayerMarkToSet);
	void RefreshMark(EMarkerType MarkerType, float RemainingDecay);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	float CachedZPosition;
	
	UFUNCTION()
	virtual void OnOwnerDestroyed(AActor* OwnerDestroyed);

	UPROPERTY()
	AMarkerStaticMesh* CrowMark;
	UPROPERTY()
	AMarkerStaticMesh* PhoenixMark;
	UPROPERTY() 
	AMarkerStaticMesh* HunterMark;

	AMarkerStaticMesh* GetActiveMark();

	bool bMarked = false;
};
