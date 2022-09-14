// Copyright (C) Nicholas Johnson 2022


#include "Components/MarkerComponent.h"
#include "Characters/BlindEyeCharacter.h"

UMarkerComponent::UMarkerComponent()
{
	CrowMark = CreateDefaultSubobject<UStaticMeshComponent>("CrowMark");
	CrowMark->SetupAttachment(this);
	PhoenixMark = CreateDefaultSubobject<UStaticMeshComponent>("PhoenixMark");
	PhoenixMark->SetupAttachment(this);
}

// Called when the game starts or when spawned
void UMarkerComponent::BeginPlay()
{
	Super::BeginPlay();
	CrowMark->SetVisibility(false); 
	CrowMark->SetVisibility(false);
}

void UMarkerComponent::PlaySpawnEffect()
{
	
}

void UMarkerComponent::RemoveMark()
{
}

void UMarkerComponent::SetPlayerMarkMesh(PlayerType PlayerMarkToSet)
{
	CrowMark->SetVisibility(PlayerMarkToSet == PlayerType::CrowPlayer);
	CrowMark->SetVisibility(PlayerMarkToSet == PlayerType::PhoenixPlayer);
}

