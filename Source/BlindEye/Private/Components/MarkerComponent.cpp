// Copyright (C) Nicholas Johnson 2022


#include "Components/MarkerComponent.h"
#include "Characters/BlindEyePlayerCharacter.h"

UMarkerComponent::UMarkerComponent()
{
}

// Called when the game starts or when spawned
void UMarkerComponent::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	FVector location = GetComponentLocation();
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Blue, "Location: " +
		FString::SanitizeFloat(location.X) + "," + FString::SanitizeFloat(location.Y) + "," + FString::SanitizeFloat(location.Z));
	CrowMark = World->SpawnActor<AStaticMeshActor>(CrowMarkType, location, GetComponentRotation());
	CrowMark->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
	PhoenixMark = World->SpawnActor<AStaticMeshActor>(PhoenixMarkType, location, GetComponentRotation());
	PhoenixMark->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
}

void UMarkerComponent::RemoveMark()
{
	// TODO: play particle effect/shader
	CrowMark->SetHidden(true);
	CrowMark->SetHidden(true);
}

void UMarkerComponent::DetonateMark()
{
	// TODO: play particle effect/shader
	CrowMark->SetHidden(true);
	CrowMark->SetHidden(true);
}

void UMarkerComponent::AddMark(PlayerType PlayerMarkToSet)
{ 
	CrowMark->SetHidden(PlayerMarkToSet == PlayerType::CrowPlayer);
	CrowMark->SetHidden(PlayerMarkToSet == PlayerType::PhoenixPlayer);
	// TODO: play particle/shader
}

