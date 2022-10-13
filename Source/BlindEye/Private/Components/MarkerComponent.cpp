// Copyright (C) Nicholas Johnson 2022


#include "Components/MarkerComponent.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "BlindEyeUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


UMarkerComponent::UMarkerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void UMarkerComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedZPosition = GetRelativeLocation().Z;

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

void UMarkerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	SetWorldLocation(GetAttachParent()->GetComponentLocation() + FVector::UpVector * CachedZPosition);

	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	APlayerController* ClientPlayerController = UGameplayStatics::GetPlayerController(World, 0);
	
	FVector ControlVecRot = ClientPlayerController->GetControlRotation().Vector();
	ControlVecRot = UKismetMathLibrary::RotateAngleAxis(ControlVecRot, 90, FVector::UpVector);
	FRotator ControlRot = ControlVecRot.Rotation();
	SetWorldRotation(FRotator(0, ControlRot.Yaw, 0));
}

void UMarkerComponent::RemoveMark()
{
	// TODO: play particle effect/shader
	CrowMark->GetStaticMeshComponent()->SetVisibility(false);
	PhoenixMark->GetStaticMeshComponent()->SetVisibility(false);
	bMarked = false;
	BP_RemoveMark();
}

void UMarkerComponent::DetonateMark()
{
	// TODO: play particle effect/shader
	CrowMark->GetStaticMeshComponent()->SetVisibility(false);
	PhoenixMark->GetStaticMeshComponent()->SetVisibility(false);
	bMarked = false;
	BP_DetonateMark();
}

void UMarkerComponent::AddMark(EPlayerType PlayerMarkToSet)
{
	if (bMarked) return;
	CrowMark->GetStaticMeshComponent()->SetVisibility(PlayerMarkToSet == EPlayerType::CrowPlayer);
	PhoenixMark->GetStaticMeshComponent()->SetVisibility(PlayerMarkToSet == EPlayerType::PhoenixPlayer);
	bMarked = true;
	BP_AddMark(PlayerMarkToSet);
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

