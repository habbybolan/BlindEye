// Copyright (C) Nicholas Johnson 2022


#include "Components/MarkerComponent.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "BlindEyeUtils.h"
#include "Net/UnrealNetwork.h"


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
	CrowMark->GetStaticMeshComponent()->SetVisibility(false);
	PhoenixMark = World->SpawnActor<AStaticMeshActor>(PhoenixMarkType, location, GetComponentRotation());
	PhoenixMark->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
	PhoenixMark->GetStaticMeshComponent()->SetVisibility(false);

	GetOwner()->OnDestroyed.AddDynamic(this, &UMarkerComponent::OnOwnerDestroyed);
} 

void UMarkerComponent::MULT_RemoveMark_Implementation()
{
	// TODO: play particle effect/shader
	CrowMark->GetStaticMeshComponent()->SetVisibility(false);
	PhoenixMark->GetStaticMeshComponent()->SetVisibility(false);
	bMarked = false;
}

void UMarkerComponent::MULT_DetonateMark_Implementation()
{
	// TODO: play particle effect/shader
	CrowMark->GetStaticMeshComponent()->SetVisibility(false);
	PhoenixMark->GetStaticMeshComponent()->SetVisibility(false);
	bMarked = false; 
}

void UMarkerComponent::MULT_AddMark_Implementation(PlayerType PlayerMarkToSet)
{
	if (bMarked) return;
	CrowMark->GetStaticMeshComponent()->SetVisibility(PlayerMarkToSet == PlayerType::CrowPlayer);
	PhoenixMark->GetStaticMeshComponent()->SetVisibility(PlayerMarkToSet == PlayerType::PhoenixPlayer);
	bMarked = true;
	// TODO: play particle/shader
}

void UMarkerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UMarkerComponent::OnOwnerDestroyed(AActor* OwnerDestroyed)
{
	if (CrowMark) CrowMark->Destroy();
	if (PhoenixMark) PhoenixMark->Destroy();
	DestroyComponent();
}

