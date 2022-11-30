// Copyright (C) Nicholas Johnson 2022


#include "Components/MarkerComponent.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "BlindEyeUtils.h"
#include "MarkerStaticMesh.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/BlindEyeGameState.h"
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
	
	FActorSpawnParameters params;
	params.Owner = GetOwner();
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	// Crow mark initialization
	CrowMark = World->SpawnActor<AMarkerStaticMesh>(CrowMarkType, location, GetComponentRotation(), params);
	CrowMark->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
	CrowMark->GetStaticMeshComponent()->SetVisibility(false);
	// phoenix mark initialization
	PhoenixMark = World->SpawnActor<AMarkerStaticMesh>(PhoenixMarkType, location, GetComponentRotation(), params);
	PhoenixMark->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
	PhoenixMark->GetStaticMeshComponent()->SetVisibility(false);
	// Hunter Mark initialization
	HunterMark = World->SpawnActor<AMarkerStaticMesh>(HunterMarkType, location, GetComponentRotation(), params);
	HunterMark->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
	HunterMark->GetStaticMeshComponent()->SetVisibility(false);

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

void UMarkerComponent::RemoveMark(EMarkerType MarkType)
{
	if (AMarkerStaticMesh* ActiveMark = GetMarkMesh(MarkType))
	{
		ActiveMark->BP_RemoveMark_CLI();
	}
	bMarked = false;
	UpdateMarkVisibility(MarkType, false);
}

void UMarkerComponent::DetonateMark(EMarkerType MarkerType)
{
	if (AMarkerStaticMesh* ActiveMark = GetMarkMesh(MarkerType))
	{
		ActiveMark->BP_DetonateMark_CLI(MarkerType);
	}
	bMarked = false;
	UpdateMarkVisibility(MarkerType, false);
}

void UMarkerComponent::AddMark(EMarkerType MarkerType)
{
	if (bMarked) return;
	bMarked = true;
	GetMarkMesh(MarkerType)->BP_AddMark(MarkerType);
	UpdateMarkVisibility(MarkerType, true);
}

void UMarkerComponent::RefreshMark(EMarkerType MarkerType, float RemainingDecay)
{
	if (AMarkerStaticMesh* ActiveMark = GetMarkMesh(MarkerType))
	{
		ActiveMark->BP_RefreshMark(MarkerType, RemainingDecay);
	}
}
 
void UMarkerComponent::UpdateMarkVisibility(EMarkerType MarkerType, bool bShowMark)
{
	CrowMark->GetStaticMeshComponent()->SetVisibility(MarkerType == EMarkerType::Crow && bShowMark);
	PhoenixMark->GetStaticMeshComponent()->SetVisibility(MarkerType == EMarkerType::Phoenix && bShowMark);
	HunterMark->GetStaticMeshComponent()->SetVisibility(MarkerType == EMarkerType::Hunter && bShowMark);
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

AMarkerStaticMesh* UMarkerComponent::GetMarkMesh(EMarkerType MarkType)
{
	switch(MarkType)
	{
	case EMarkerType::Crow:
		return CrowMark;
	case EMarkerType::Phoenix:
		return PhoenixMark;
	case EMarkerType::Hunter:
		return HunterMark;
	default:
		return CrowMark;
	}
}

