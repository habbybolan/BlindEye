// Copyright (C) Nicholas Johnson 2022


#include "Abilities/PhoenixFireballCast.h"

#include "NiagaraFunctionLibrary.h"

// Sets default values
APhoenixFireballCast::APhoenixFireballCast()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComponent->SetEnableGravity(false);
	RootComponent = SphereComponent;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("mesh");
	Mesh->SetupAttachment(RootComponent);
	
	Movement = CreateDefaultSubobject<UProjectileMovementComponent>("Movement");
	Movement->MaxSpeed = 1000;
	Movement->InitialSpeed = 0;
	Movement->ProjectileGravityScale = 0;
}

USphereComponent* APhoenixFireballCast::GetSphereComponent()
{
	return SphereComponent;
}

// Called when the game starts or when spawned
void APhoenixFireballCast::BeginPlay()
{
	Super::BeginPlay();
	Movement->Velocity = GetActorForwardVector() * FireballSpeed;
	MULT_SpawnFireballTrail_Implementation();
}

void APhoenixFireballCast::MULT_SpawnFireballTrail_Implementation()
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	SpawnedFireTrailParticle = UNiagaraFunctionLibrary::SpawnSystemAttached(FireTrailParticle, GetRootComponent(), NAME_None,
		GetActorLocation(), GetActorRotation(), FVector::OneVector,
		EAttachLocation::KeepWorldPosition, false, ENCPoolMethod::AutoRelease);
}

