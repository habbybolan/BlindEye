// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "BurrowerSpawnPoint.generated.h"

enum class EIslandPosition : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLINDEYE_API UBurrowerSpawnPoint : public USceneComponent
{
	GENERATED_BODY()
	
public:

	UBurrowerSpawnPoint();

	UPROPERTY(EditAnywhere)
	uint8 IslandID;

	bool bInUse = false;

};
