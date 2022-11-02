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
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Blue, "Location: " +
		FString::SanitizeFloat(location.X) + "," + FString::SanitizeFloat(location.Y) + "," + FString::SanitizeFloat(location.Z));
	
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

void UMarkerComponent::RemoveMark()
{
	if (AMarkerStaticMesh* ActiveMark = GetActiveMark())
	{
		ActiveMark->BP_RemoveMark();
		ActiveMark->GetStaticMeshComponent()->SetVisibility(false);
	}
	bMarked = false;
	
}

void UMarkerComponent::DetonateMark(EMarkerType MarkerType)
{
	if (AMarkerStaticMesh* ActiveMark = GetActiveMark())
	{
		ActiveMark->BP_DetonateMark(MarkerType);
		ActiveMark->GetStaticMeshComponent()->SetVisibility(false);
	}
	bMarked = false;

	// Marker logic for tutorial
	if (MarkerType == EMarkerType::Crow)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
			for (APlayerState* PlayerState : BlindEyeGS->PlayerArray)
			{
				if (ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PlayerState->GetPawn()))
				{
					if (Player->PlayerType == EPlayerType::CrowPlayer && MarkerType == EMarkerType::Phoenix ||
						Player->PlayerType == EPlayerType::PhoenixPlayer && MarkerType == EMarkerType::Crow)
					{
						Player->CLI_TryFinishTutorial(ETutorialChecklist::Detonate);
					}
				}
			}
		}
	}
}

void UMarkerComponent::AddMark(EMarkerType MarkerType)
{
	if (bMarked) return;
	CrowMark->GetStaticMeshComponent()->SetVisibility(MarkerType == EMarkerType::Crow);
	PhoenixMark->GetStaticMeshComponent()->SetVisibility(MarkerType == EMarkerType::Phoenix);
	HunterMark->GetStaticMeshComponent()->SetVisibility(MarkerType == EMarkerType::Hunter);
	bMarked = true;
	GetActiveMark()->BP_AddMark(MarkerType);

	// Marker logic for tutorial
	UWorld* World = GetWorld();
	if (World)
	{
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
		for (APlayerState* PlayerState : BlindEyeGS->PlayerArray)
		{
			if (ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PlayerState->GetPawn()))
			{
				if (Player->PlayerType == EPlayerType::CrowPlayer && MarkerType == EMarkerType::Crow ||
					Player->PlayerType == EPlayerType::PhoenixPlayer && MarkerType == EMarkerType::Phoenix)
				{
					Player->CLI_TryFinishTutorial(ETutorialChecklist::MarkEnemy);
				}
			}
		}
	}
}

void UMarkerComponent::RefreshMark(EMarkerType MarkerType)
{
	if (AMarkerStaticMesh* ActiveMark = GetActiveMark())
	{
		ActiveMark->BP_RefreshMark(MarkerType);
	}
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

AMarkerStaticMesh* UMarkerComponent::GetActiveMark()
{
	if (CrowMark->GetStaticMeshComponent()->IsVisible()) return CrowMark;
	if (PhoenixMark->GetStaticMeshComponent()->IsVisible()) return PhoenixMark;
	if (HunterMark->GetStaticMeshComponent()->IsVisible()) return HunterMark;
	return nullptr;
}

