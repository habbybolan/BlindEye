// Copyright (C) Nicholas Johnson 2022


#include "Abilities/PhoenixFireball.h"

#include "NiagaraFunctionLibrary.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Characters/BlindEyePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


APhoenixFireball::APhoenixFireball() : AAbilityBase()
{
	AbilityStates.Add(new FStartCastingAbilityState(this));
	AbilityStates.Add(new FCastFireballState(this));
	AbilityType = EAbilityTypes::Unique1;
}

void APhoenixFireball::DealWithDamage(AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit, float DamageToApply)
{
	if (OtherActor == nullptr) return;
	// prevent damage on same actor twice
	if (IDsOfHitActors.Contains(OtherActor->GetUniqueID())) return;
	
	IDsOfHitActors.Add(OtherActor->GetUniqueID());
	UGameplayStatics::ApplyPointDamage(OtherActor, DamageToApply, NormalImpulse, Hit, GetInstigator()->GetController(), GetInstigator(), DamageType);
}

void APhoenixFireball::CastFireCone()
{
	UWorld* world = GetWorld();
	if (!world) return;

	FVector EndLocation;

	if (GetIsTopdown())
	{
		GetMouseTargetLocationHelper(EndLocation, TArray<TEnumAsByte<EObjectTypeQuery>>(), false);
	} else
	{
		FVector ViewportLocation;
		FRotator ViewportRotation;
		GetInstigator()->GetController()->GetPlayerViewPoint(OUT ViewportLocation, OUT ViewportRotation);

		EndLocation = ViewportLocation + ViewportRotation.Vector() * 1000;
		FHitResult OutHit;
		if (UKismetSystemLibrary::LineTraceSingleForObjects(world, ViewportLocation, EndLocation, LineTraceObjectTypes, false,
			TArray<AActor*>(), EDrawDebugTrace::None, OutHit, true))
		{
			EndLocation = OutHit.Location;
		}
	}

	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects(world, GetInstigator()->GetActorLocation(), EndLocation, FVector(0, ConeWidth / 2, ConeLength / 2),
		GetInstigator()->GetControlRotation(), ConeTraceObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::None,
		OutHits, true);

	FVector UserLocation = GetInstigator()->GetActorLocation();
	for (FHitResult ConeHit : OutHits)
	{
		// some surfaces return null for actor
		if (ConeHit.GetActor() == nullptr) return;
		
		// check if hit result in boxTrace is within the player's cone
		float AngleToUser = acos(UKismetMathLibrary::Dot_VectorVector(UserLocation, ConeHit.Location) / (UserLocation.Size() * ConeHit.Location.Size()));
		if (abs(AngleToUser) <= ConeHalfAngleDeg)
		{
			DealWithDamage(ConeHit.GetActor(), ConeHit.ImpactNormal, ConeHit, Damage);
		}
	}
}

void APhoenixFireball::CastFireball()
{
	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters params;
	params.Instigator = GetInstigator();
	params.Owner = this;

	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(GetInstigator());
	FVector SpawnLocation = Player->GetMesh()->GetBoneLocation("RightHand") + Player->GetActorForwardVector() * 25;

	// Find Rotation of phoenix fireball
	FVector vectorRotation = FVector::ZeroVector;

	// If Topdown
	if (Player->GetIsTopdown())
	{
		vectorRotation = GetTargetRotationHelper().Vector();
	}
	else
	{
		FVector ViewportLocation;
		FRotator ViewportRotation;
		GetInstigator()->GetController()->GetPlayerViewPoint(ViewportLocation, ViewportRotation);
		
		FHitResult Hit;
		if (UKismetSystemLibrary::LineTraceSingleForObjects(World, ViewportLocation, ViewportLocation + ViewportRotation.Vector() * 10000, FireballCastObjectTypes,
			false, TArray<AActor*>(), EDrawDebugTrace::None, Hit, true))
		{
			vectorRotation = Hit.Location - SpawnLocation;
		} else
		{
			vectorRotation = (ViewportLocation + ViewportRotation.Vector() * 10000) - SpawnLocation;
		}
	}
	
	FireballCast = World->SpawnActor<APhoenixFireballCast>(FireballCastType, SpawnLocation, vectorRotation.Rotation(), params);
	FireballCast->CustomCollisionDelegate.BindDynamic(this, &APhoenixFireball::OnFireballCastHit);
}

void APhoenixFireball::OnFireballCastHit()
{
	if (GetOwner()->GetLocalRole() < ROLE_Authority) return;
	
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	if (!FireballCast) return;

	// Area damage from fireball cast colliding / expiring and exploding
	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::SphereTraceMultiForObjects(World, FireballCast->GetActorLocation(), FireballCast->GetActorLocation() + FireballCast->GetActorForwardVector() * 5,
		FireballExplosionRadius, ConeTraceObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::None, OutHits, true);
	for (FHitResult ExplosionHit : OutHits) 
	{
		DealWithDamage(ExplosionHit.Actor.Get(), ExplosionHit.ImpactNormal, ExplosionHit, Damage);
	}
}

void APhoenixFireball::PlayAbilityAnimation()
{
	StartLockRotation(1);
	PlayAbilityAnimationHelper();
	AnimNotifyDelegate.BindUFunction( this, TEXT("UseAnimNotifyExecuted"));
}

void APhoenixFireball::PlayAbilityAnimationHelper()
{
	if (ABlindEyePlayerCharacter* PlayerCharacter = Cast<ABlindEyePlayerCharacter>(GetOwner()))
	{
		PlayerCharacter->PlayAnimMontage(FireballCastAnimation);
	}
}

void APhoenixFireball::MULT_PlayAbilityAnimation_Implementation()
{
	// Play for remote clients
	if (!GetIsLocallyControlled())
	{
		PlayAbilityAnimationHelper();
	}
}

void APhoenixFireball::UseAnimNotifyExecuted()
{
	AnimNotifyDelegate.Unbind();
	AbilityStates[CurrState]->ExitState();
	AnimNotifyDelegate.BindUFunction(this, TEXT("EndAnimationNotifyExecuted"));
}

void APhoenixFireball::EndAnimationNotifyExecuted()
{
	AnimNotifyDelegate.Unbind();
	AbilityStates[CurrState]->ExitState();
}

void APhoenixFireball::EndAbilityLogic()
{
	AAbilityBase::EndAbilityLogic();
	StopLockRotation();
	
	IDsOfHitActors.Empty();
}

// **** States *******

// Start playing animation state *********************

FStartCastingAbilityState::FStartCastingAbilityState(AAbilityBase* ability) : FAbilityState(ability) {}

void FStartCastingAbilityState::TryEnterState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	FAbilityState::TryEnterState(abilityUsageType, Location, Rotation);
	if (!Ability) return;
	if (APhoenixFireball* PhoenixFireball = Cast<APhoenixFireball>(Ability))
	{
		if (!PhoenixFireball->TryConsumeBirdMeter(PhoenixFireball->CostPercent)) return;
	}
	if (abilityUsageType != EAbilityInputTypes::Pressed) return;
	Ability->SetOnCooldown();
	RunState();
}

void FStartCastingAbilityState::RunState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	// prevent input in this state
	if (abilityUsageType > EAbilityInputTypes::None) return;
	
	FAbilityState::RunState(abilityUsageType, Location, Rotation);
	if (!Ability) return;
	Ability->AbilityStarted();
	
	APhoenixFireball* PhoenixFireball = Cast<APhoenixFireball>(Ability);
	
	// blockers
	PhoenixFireball->Blockers.IsOtherAbilitiesBlocked = true;
	PhoenixFireball->Blockers.IsMovementSlowBlocked = true;
	PhoenixFireball->Blockers.MovementSlowAmount = PhoenixFireball->SlowAmount;
	
	PhoenixFireball->PlayAbilityAnimation();
	PhoenixFireball->MULT_PlayAbilityAnimation();
}

void FStartCastingAbilityState::ExitState()
{
	FAbilityState::ExitState();
	if (Ability == nullptr) return;
	Ability->AbilityInnerState(1);
	Ability->EndCurrState();
	Ability->UseAbility(EAbilityInputTypes::None);
}

bool FStartCastingAbilityState::CancelState()
{
	FAbilityState::CancelState();

	APhoenixFireball* PhoenixFireball = Cast<APhoenixFireball>(Ability);
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PhoenixFireball->GetOwner());
	Player->MULT_StopAnimMontage(PhoenixFireball->FireballCastAnimation);
	return true;
}

// Casting fireball state *********************
 
FCastFireballState::FCastFireballState(AAbilityBase* ability) : FAbilityState(ability) {}

void FCastFireballState::TryEnterState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	FAbilityState::TryEnterState(abilityUsageType, Location, Rotation);
	RunState();
}

void FCastFireballState::RunState(EAbilityInputTypes abilityUsageType, const FVector& Location, const FRotator& Rotation)
{
	// prevent input in this state
	if (abilityUsageType > EAbilityInputTypes::None) return;
	
	FAbilityState::RunState(abilityUsageType, Location, Rotation);
	if (Ability == nullptr) return;

	APhoenixFireball* PhoenixFireball = Cast<APhoenixFireball>(Ability);
	if (!PhoenixFireball) return;

	// blockers
	PhoenixFireball->Blockers.IsOtherAbilitiesBlocked = true;
	PhoenixFireball->Blockers.IsMovementSlowBlocked = true;
	PhoenixFireball->Blockers.MovementSlowAmount = PhoenixFireball->SlowAmount;

	PhoenixFireball->CastFireball();
	PhoenixFireball->CastFireCone();
}

void FCastFireballState::ExitState()
{
	FAbilityState::ExitState();
	Ability->EndCurrState();
}

bool FCastFireballState::CancelState()
{
	FAbilityState::CancelState();

	APhoenixFireball* PhoenixFireball = Cast<APhoenixFireball>(Ability);
	ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PhoenixFireball->GetOwner());
	Player->MULT_StopAnimMontage(PhoenixFireball->FireballCastAnimation);
	return true;
}


