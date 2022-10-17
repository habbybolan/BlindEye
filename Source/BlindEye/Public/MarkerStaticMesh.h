// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MarkerStaticMesh.generated.h"

enum class EMarkerType : uint8;

/**
 * 
 */
UCLASS()
class BLINDEYE_API AMarkerStaticMesh : public AStaticMeshActor
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void BP_RemoveMark();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_DetonateMark(EMarkerType MarkerType); 
	UFUNCTION(BlueprintImplementableEvent)
	void BP_AddMark(EMarkerType MarkerType);
	UFUNCTION(BlueprintImplementableEvent)
	void BP_RegenerateMark();
	
};
