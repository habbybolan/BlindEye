// Copyright (C) Nicholas Johnson 2022


#include "Characters/TopdownCameraManager.h"

#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"

UTopdownCameraManager::UTopdownCameraManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PostUpdateWork;
}

void UTopdownCameraManager::InitializeTopdownCamera(UCameraComponent* followCamera)
{
	FollowCamera = followCamera;

	// detach from player for manual movement
	//const FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, false);
	//FollowCamera->DetachFromComponent(DetachmentRules);

	// start manually updating camera position
	SetComponentTickEnabled(true);
}

void UTopdownCameraManager::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
}

void UTopdownCameraManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateTopdownCamera();
}

void UTopdownCameraManager::UpdateTopdownCamera()
{
	// Camera location
	FRotator WorldRotationCameraOutOffset = FRotator(0, StartingWorldZAngleOfCamera, 0);
	FVector CameraPos = GetOwner()->GetActorLocation() +
						FVector::UpVector * StartingCameraHeightOffset +
						WorldRotationCameraOutOffset.Vector() * StartingCameraOutOffset;
	FollowCamera->SetWorldLocation(CameraPos);
	
	// camera rotation
	FRotator CameraRot = UKismetMathLibrary::FindLookAtRotation(CameraPos, GetOwner()->GetActorLocation());
	FollowCamera->SetWorldRotation(CameraRot);
}

