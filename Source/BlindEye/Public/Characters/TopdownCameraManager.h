// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TopdownCameraManager.generated.h"


class UCameraComponent;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLINDEYE_API UTopdownCameraManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTopdownCameraManager();

	void InitializeTopdownCamera(UCameraComponent* followCamera);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float StartingCameraHeightOffset = 850.f;
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float StartingCameraOutOffset = 650.f;
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float StartingWorldZAngleOfCamera = 50;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	class UCameraComponent* FollowCamera;

	void UpdateTopdownCamera();
		
};
