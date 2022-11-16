// Copyright (C) Nicholas Johnson 2022


#include "Abilities/PhoenixFireballCast.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APhoenixFireballCast::APhoenixFireballCast()
{
	PrimaryActorTick.bCanEverTick = false;
	InitialLifeSpan = 0; // infinite lifespan
	
	bReplicates = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComponent->SetEnableGravity(false);
	RootComponent = SphereComponent;
	
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("mesh");
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
	//MULT_SpawnFireballTrail_Implementation();

	SphereComponent->OnComponentHit.AddDynamic(this, &APhoenixFireballCast::OnCollision);

	UWorld* world = GetWorld();
	if (world)
	{
		world->GetTimerManager().SetTimer(LifespanTimerHandle, this, &APhoenixFireballCast::CollisionLogic, CustomLifespan, false);
	}
}

void APhoenixFireballCast::Destroyed()
{
	Super::Destroyed();
} 

void APhoenixFireballCast::MULT_SpawnFireballTrail_Implementation()
{
	UWorld* world = GetWorld();
	if (!world) return;
	
	// SpawnedFireTrailParticle = UNiagaraFunctionLibrary::SpawnSystemAttached(FireTrailParticle, GetRootComponent(), NAME_None,
	// 	GetActorLocation(), GetActorRotation(), FVector::OneVector,
	// 	EAttachLocation::KeepWorldPosition, false, ENCPoolMethod::AutoRelease);
}

void APhoenixFireballCast::DelayedDestruction()
{
	Destroy();
}

void APhoenixFireballCast::HideFireball() 
{
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetHiddenInGame(true);
	BP_OnCollision_CLI();
	//SpawnedFireTrailParticle->Deactivate();
}


void APhoenixFireballCast::BurnLogic()
{
	UWorld* world = GetWorld();
	if (!world) return;

	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::SphereTraceMultiForObjects(world, BurnLocation, BurnLocation + FVector::UpVector, BurningRadius,
		BurnObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::None, OutHits, true);
	for (FHitResult Hit : OutHits)
	{ 
		UGameplayStatics::ApplyPointDamage(Hit.GetActor(), BurningBaseDamagePerSec * BurnDamageDelay, FVector::ZeroVector,
			Hit, GetInstigator()->GetController(),GetInstigator(), BurnDamageType);
	}
}

void APhoenixFireballCast::OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	CollisionLogic();
}

void APhoenixFireballCast::CollisionLogic()
{
	UWorld* world = GetWorld();
	if (!world) return;

	BP_Explosion_CLI();
	
	world->GetTimerManager().ClearTimer(LifespanTimerHandle);
	
	HideFireball();

	TArray<AActor*> ignoreActors;
	ignoreActors.Add(this);
	// Check if close enough to ground to perform burn on ground
	FHitResult HitResult;
	if (UKismetSystemLibrary::LineTraceSingleForObjects(world, GetActorLocation(), GetActorLocation() + FVector::DownVector * MaxHeightToApplyFire,
		LineTraceObjectTypes, false, ignoreActors, EDrawDebugTrace::None, HitResult, true))
	{
		BurnLocation = HitResult.Location;
		BP_GroundBurning_CLI(BurnLocation, BurningDuration);

		// Apply burning damage
		if (GetLocalRole() == ROLE_Authority)
		{
			world->GetTimerManager().SetTimer(BurnTimerHandle, this, &APhoenixFireballCast::BurnLogic, BurnDamageDelay, true);
		}
	}

	// Destroy after burning finished
	world->GetTimerManager().SetTimer(DelayedDestroyTimerHandle, this, &APhoenixFireballCast::DelayedDestruction, BurningDuration, false);
}

