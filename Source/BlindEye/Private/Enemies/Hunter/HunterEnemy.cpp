// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Hunter/HunterEnemy.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BlindEyePlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemies/Hunter/HunterEnemyController.h"
#include "Enemies/Hunter/HunterHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

AHunterEnemy::AHunterEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UHunterHealthComponent>(TEXT("HealthComponent")))
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AHunterEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Slow movement if close to target and target is marked
	bool bMovementUpdated = false;
	if (IsTargetMarked())
	{
		if (AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(GetController()))
		{
			if (AActor* Target = HunterController->GetBTTarget())
			{
				if (FVector::Distance(Target->GetActorLocation(), GetActorLocation()) < DistToMarkedPlayerToSlowDown)
				{
					bMovementUpdated = true;
					GetCharacterMovement()->MaxWalkSpeed = CachedRunningSpeed * MovementSpeedSlowWhenCloseToMarkedPlayer;
				}
			}
		}
	}
	
	// To make sure it doesn't get stuck on slow speed
	if (!bMovementUpdated)
	{
		GetCharacterMovement()->MaxWalkSpeed = CachedRunningSpeed;
	}
}

void AHunterEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	CachedRunningSpeed = GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed = bCharged ? CachedRunningSpeed : CachedRunningSpeed * MovementSpeedAlteredDuringNotCharged;

	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	if (AActor* ShrineActor = UGameplayStatics::GetActorOfClass(World, AShrine::StaticClass()))
	{
		Shrine = Cast<AShrine>(ShrineActor);
	}

	GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &AHunterEnemy::AnimMontageEnded);
	Material = GetMesh()->CreateDynamicMaterialInstance(0, DissolveMaterial);
}

void AHunterEnemy::Despawn()
{
	BP_HunterDespawned_SER();
}

void AHunterEnemy::PerformChargedJump()
{
	if (ABlindEyeEnemyController* BlindEyeController = Cast<ABlindEyeEnemyController>(GetController()))
	{
		if (AActor* Target = BlindEyeController->GetBTTarget())
		{
			// Have target position land before the target
			FVector DirectionVec = Target->GetActorLocation() - GetActorLocation();
			DirectionVec.Normalize();
			DirectionVec *= ChargedJumpLandingDistanceBeforeTarget;

			FVector JumpTargetLocation = Target->GetActorLocation() - DirectionVec;
			
			if (UWorld* World = GetWorld())
			{
				TArray<AActor*> ActorsToIgnore;
				ActorsToIgnore.Add(Target);
				FHitResult HitResult;
				
				// If no environment blocking LOS, then perform jump
				if (!UKismetSystemLibrary::LineTraceSingleForObjects(World, GetActorLocation(), JumpTargetLocation, ChargedJumpLOSBlockers, false,
					ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true))
				{
					CurrAttack = EHunterAttacks::ChargedJump;
					bChargeAttackCooldown = true;
			
					GetWorldTimerManager().SetTimer(ChargedJumpCooldownTimerHandle, this, &AHunterEnemy::SetChargedJumpOffCooldown, ChargedJumpCooldown, false);
					MULT_PerformChargedJumpHelper(GetActorLocation(), JumpTargetLocation);
				}
			}
		}
	}
}

void AHunterEnemy::MULT_PerformChargedJumpHelper_Implementation(FVector StartLoc, FVector EndLoc)
{
	PlayAnimMontage(ChargedJumpAnim);
	ChargedJumpDuration = 1; // Duration of the jump part of the animation 
	// Set Start and end locations of jump for Easing
	ChargedJumpStartLocation = StartLoc;
	ChargedJumpTargetLocation = EndLoc;

	float JumpDistance = FVector::Distance(StartLoc, EndLoc);
	// Calculate the height of the jump attack based on dist. to player
	ChargedJumpPeakHeight = UKismetMathLibrary::Max(ChargedJumpMaxPeakHeight *
		((JumpDistance - MinDistanceToChargeJump) / (MaxDistanceToChargeJump - MinDistanceToChargeJump)), ChargedJumpMinPeakHeight);
	
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetWorldTimerManager().SetTimer(PerformingChargedJumpTimerHandle, this, &AHunterEnemy::PerformingJumpAttack, ChargedJumpCalcDelay, true);
	GetWorldTimerManager().SetTimer(ChargedJumpRotationTimerHandle, this, &AHunterEnemy::RotateDuringChargedAttack, ChargedJumpCalcDelay, true);
}

void AHunterEnemy::PerformingJumpAttack()
{ 
	FVector ForwardEase = UKismetMathLibrary::VEase(ChargedJumpStartLocation * FVector(1, 1, 0),
		ChargedJumpTargetLocation* FVector(1, 1, 0), CurrTimeOfChargedJump / ChargedJumpDuration, EEasingFunc::Linear);

	FVector UpEase;
	FVector HalfDirectionToTarget = (ChargedJumpTargetLocation - ChargedJumpStartLocation) / 2 + FVector::UpVector * ChargedJumpPeakHeight;
	float HalfChargedAttackDuration = ChargedJumpDuration / 2;
	// If at first half of jump, jump from starting Z to jump Z-Peak
	if (CurrTimeOfChargedJump / ChargedJumpDuration <= 0.5)
	{
		 UpEase = UKismetMathLibrary::VEase(ChargedJumpStartLocation * FVector::UpVector,
			(ChargedJumpTargetLocation + HalfDirectionToTarget) * FVector::UpVector, CurrTimeOfChargedJump / HalfChargedAttackDuration, EEasingFunc::CircularOut);
	}
	// Other latter half of jump, Go from Jump Z-Peak to end point Z
	else
	{
		UpEase = UKismetMathLibrary::VEase((ChargedJumpTargetLocation + HalfDirectionToTarget) * FVector::UpVector,
		   ChargedJumpTargetLocation * FVector::UpVector,
		   (CurrTimeOfChargedJump - HalfChargedAttackDuration) / (ChargedJumpDuration - HalfChargedAttackDuration), EEasingFunc::CircularIn);
	}

	SetActorLocation(ForwardEase + UpEase);
	CurrTimeOfChargedJump += ChargedJumpCalcDelay;

	if (CurrTimeOfChargedJump >= ChargedJumpDuration)
	{
		GetWorldTimerManager().ClearTimer(PerformingChargedJumpTimerHandle);
		GetWorldTimerManager().ClearTimer(ChargedJumpRotationTimerHandle);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CurrTimeOfChargedJump = 0;
	}
}

void AHunterEnemy::RotateDuringChargedAttack()
{
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ChargedJumpTargetLocation);
	FRotator NewRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), TargetRotation, ChargedJumpCalcDelay, ChargedJumpRInterpSpeed);
	NewRotation.Roll = 0;
	NewRotation.Pitch = 0;
	SetActorRotation(NewRotation);
}

void AHunterEnemy::SetCharged()
{
	UWorld* World = GetWorld();
	if (!ensure(World)) return;

	bCharged = true;
	BP_ChargedStarted();

	GetCharacterMovement()->MaxWalkSpeed = CachedRunningSpeed;

	World->GetTimerManager().ClearTimer(ChargedCooldownTimerHandle);
	if (ChargedDuration != 0)
	{
		World->GetTimerManager().SetTimer(ChargedDurationTimerHandle, this, &AHunterEnemy::SetNotCharged, ChargedDuration, false);
	}
}

void AHunterEnemy::SetNotCharged()
{
	UWorld* World = GetWorld();
	if (!ensure(World)) return;

	GetCharacterMovement()->MaxWalkSpeed = CachedRunningSpeed * MovementSpeedAlteredDuringNotCharged;
	
	bCharged = false;
	BP_ChargedEnded();
	
	World->GetTimerManager().ClearTimer(ChargedDurationTimerHandle);
	//World->GetTimerManager().SetTimer(ChargedCooldownTimerHandle, this, &AHunterEnemy::SetCharged, ChargedCooldown, false);
}

void AHunterEnemy::OnMarkDetonated(AActor* MarkedPawn, EMarkerType MarkerType)
{
	Super::OnMarkDetonated(MarkedPawn, MarkerType);
	if (GetIsCharged())
	{
		AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(Controller);
		check(HunterController);
		
		AActor* Target = HunterController->GetBTTarget();
		HealthComponent->Stun(5, Target);
		SetNotCharged();
	}
}

void AHunterEnemy::OnMarkAdded(AActor* MarkedActor, EMarkerType MarkerType)
{
	Super::OnMarkAdded(MarkedActor, MarkerType);
	if (bDebugStunOnMark)
	{
		if (GetIsCharged())
		{
			AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(Controller);
			check(HunterController);
		
			AActor* Target = HunterController->GetBTTarget();
			HealthComponent->Stun(5, Target);
			SetNotCharged();
		}
	}
}

void AHunterEnemy::RemoveHunterMarkOnPlayer()
{
	UWorld* World = GetWorld();
	if (!ensure(World)) return;
	
	// Remove any hunter mark on any of the players 
	AGameStateBase* GameState = UGameplayStatics::GetGameState(World);
	TArray<APlayerState*> LocalPlayers = GameState->PlayerArray;
	for (APlayerState* LocalPlayer : LocalPlayers)
	{
		APawn* PlayerPawn = LocalPlayer->GetPawn();
		ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(PlayerPawn);
		check(Player);
		// remove the mark from player if it's a hunter mark
		FMarkData Mark =  Player->GetHealthComponent()->GetCurrMark();
		if (Mark.bHasMark)
		{
			if (Mark.MarkerType == EMarkerType::Hunter)
			{
				Player->GetHealthComponent()->RemoveMark();
				return;
			}
		}
	}
}

void AHunterEnemy::PerformBasicAttack()
{
	CurrAttack = EHunterAttacks::BasicAttack;
	MULT_PerformBasicAttackHelper(bLeftBasicAttack ? BasicAttackLeftAnimation : BasicAttackRightAnimation);
	bLeftBasicAttack = !bLeftBasicAttack;
}

void AHunterEnemy::MULT_PerformBasicAttackHelper_Implementation(UAnimMontage* AnimToPlay)
{
	PlayAnimMontage(AnimToPlay);
}

void AHunterEnemy::OnHunterMarkDetonated(AActor* MarkedPawn, EMarkerType MarkerType)
{
	GetMesh()->GetAnimInstance()->StopAllMontages(0);
	UnsubscribeToTargetMarks();
	AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(Controller);
	check(HunterController);
	AActor* Target = HunterController->GetBTTarget();
	
	HealthComponent->Stun(5, Target);
}

void AHunterEnemy::OnHunterMarkRemoved(AActor* UnmarkedActor, EMarkerType MarkerType)
{
	GetMesh()->GetAnimInstance()->StopAllMontages(0);
	UnsubscribeToTargetMarks();
}

void AHunterEnemy::OnMarkedPlayerDied(AActor* PlayerKilled)
{
	PlayAnimMontage(RoarAnimation);
	AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(Controller);
	ensure(HunterController);
	HunterController->OnMarkedPlayerDeath();
}

void AHunterEnemy::UnsubscribeToTargetMarks()
{
	AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(Controller);
	check(HunterController);
	AActor* Target = HunterController->GetBTTarget();
	if (Target)
	{
		bPlayerMarked = false;
		ABlindEyePlayerCharacter* Player = Cast<ABlindEyePlayerCharacter>(Target);
		check(Player);
		Player->GetHealthComponent()->DetonateDelegate.Remove(this, TEXT("OnHunterMarkDetonated"));
		Player->GetHealthComponent()->MarkedRemovedDelegate.Remove(this, TEXT("OnHunterMarkRemoved"));
		Player->GetHealthComponent()->OnDeathDelegate.Remove(this, TEXT("OnMarkedPlayerDied"));
	}
} 

void AHunterEnemy::TrySetVisibility(bool visibility)
{
	IsVisible = visibility;
	OnRep_IsVisible();
}

void AHunterEnemy::OnRep_IsVisible()
{
	BP_SetVisibility_CLI(IsVisible);
}
 
void AHunterEnemy::OnDeath(AActor* ActorThatKilled)
{
	Super::OnDeath(ActorThatKilled);
	GetMesh()->GetAnimInstance()->StopAllMontages(.25);
	if (AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(GetController()))
	{
		HunterController->OnHunterDeath(nullptr);
	}
	RemoveHunterMarkOnPlayer();
	UnPossessed();
}

void AHunterEnemy::SetChargedJumpOffCooldown()
{
	bChargeAttackCooldown = false;
	
	UWorld* World = GetWorld();
	if (World) World->GetTimerManager().ClearTimer(ChargedJumpCooldownTimerHandle);
}

bool AHunterEnemy::GetIsChargedJumpOnCooldown()
{
	return bChargeAttackCooldown;
}

bool AHunterEnemy::GetIsAttacking()
{
	return CurrAttack > EHunterAttacks::None;
}

EHunterAttacks AHunterEnemy::GetCurrAttack()
{
	return CurrAttack;
}

bool AHunterEnemy::GetIsCharged()
{
	return bCharged;
}

bool AHunterEnemy::GetIsChannelling()
{
	return bChannelling;
}

void AHunterEnemy::ApplyBasicAttackDamage(FHitResult Hit, bool IfShouldApplyHunterMark)
{
	// base DamageType and damage amount if Hunter Charged and if marking target
	TSubclassOf<UBaseDamageType> DamageTypeToApply = IfShouldApplyHunterMark ? BasicAttackDamageTypeWithMark : BasicAttackDamageTypeNoMark;
	ApplyAttackDamageHelper(BasicAttackDamage, IfShouldApplyHunterMark, DamageTypeToApply, Hit);
}

void AHunterEnemy::ApplyChargedJumpDamage(FHitResult Hit, bool IfShouldApplyHunterMark)
{
	// base DamageType and damage amount if Hunter Charged and if marking target
	TSubclassOf<UBaseDamageType> DamageTypeToApply = IfShouldApplyHunterMark ? ChargedJumpDamageTypeWithMark : ChargedJumpDamageTypeNoMark;
	ApplyAttackDamageHelper(ChargedAttackDamage, IfShouldApplyHunterMark, DamageTypeToApply, Hit);
}

void AHunterEnemy::ApplyAttackDamageHelper(float Damage, bool IfShouldApplyHunterMark,
	TSubclassOf<UBaseDamageType> DamageType, FHitResult Hit)
{
	UGameplayStatics::ApplyPointDamage(Hit.Actor.Get(), Damage, Hit.ImpactNormal, Hit, GetController(), this, DamageType);

	if (!bPlayerMarked && IfShouldApplyHunterMark)
	{
		SetPlayerMarked(Hit.Actor.Get());
	}
}

void AHunterEnemy::StartChanneling()
{
	if (bChannelling) return;

	MULT_StartChannelingHelper();
	check(Shrine);
	Shrine->ChannelingStarted(this);
}

void AHunterEnemy::MULT_StartChannelingHelper_Implementation()
{
	PlayAnimMontage(ChannelingAnim);
	UWorld* World = GetWorld();
	if (ensure(World))
	{
		bChannelling = true;
		World->GetTimerManager().SetTimer(ChannellingTimerHandle, this, &AHunterEnemy::StopChanneling, ChannellingDuration, false);
	}
	BP_ChannelingStarted_CLI();
}

void AHunterEnemy::StopChanneling()
{
	check(Shrine);
	Shrine->ChannellingEnded(this);
	GetMesh()->GetAnimInstance()->Montage_JumpToSection("End", ChannelingAnim);
	// TODO: Set Channeling Anim to End section
	BP_ChannelingEnded_CLI();
}

void AHunterEnemy::ChannelingAnimFinished()
{
	bChannelling = false;
	SetCharged();
}

void AHunterEnemy::AnimMontageEnded(UAnimMontage* Montage, bool bInterrupted) 
{
	// If attacking animation
	if (Montage == ChargedJumpAnim || Montage == BasicAttackLeftAnimation || Montage == BasicAttackRightAnimation )
	{
		CurrAttack = EHunterAttacks::None;
		GetCharacterMovement()->MaxWalkSpeed = CachedRunningSpeed;
	}
}

void AHunterEnemy::OnStunStart(float StunDuration)
{
	SetNotCharged();
	RemoveHunterMarkOnPlayer();
	GetMesh()->GetAnimInstance()->StopAllMontages(.2);
}

bool AHunterEnemy::GetIsFleeing()
{ 
	return bFleeing;
}

void AHunterEnemy::SetPlayerMarked(AActor* NewTarget)
{
	if (GetLocalRole() < ROLE_Authority) return;
	AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(GetController());
	if (!ensure(HunterController)) return;
	
	ABlindEyePlayerCharacter* BlindEyePlayerCharacter = Cast<ABlindEyePlayerCharacter>(NewTarget);
	if (ensure(BlindEyePlayerCharacter))
	{
		bPlayerMarked = true;
		if (!BlindEyePlayerCharacter->GetHealthComponent()->DetonateDelegate.Contains(this, TEXT("OnHunterMarkDetonated")))
		{
			BlindEyePlayerCharacter->GetHealthComponent()->DetonateDelegate.AddDynamic(this, &AHunterEnemy::OnHunterMarkDetonated);
		}
		if (!BlindEyePlayerCharacter->GetHealthComponent()->MarkedRemovedDelegate.Contains(this, TEXT("OnHunterMarkRemoved")))
		{
			BlindEyePlayerCharacter->GetHealthComponent()->MarkedRemovedDelegate.AddDynamic(this, &AHunterEnemy::OnHunterMarkRemoved);
		}
		if (!BlindEyePlayerCharacter->GetHealthComponent()->OnDeathDelegate.Contains(this, TEXT("OnMarkedPlayerDied")))
		{
			BlindEyePlayerCharacter->GetHealthComponent()->OnDeathDelegate.AddDynamic(this, &AHunterEnemy::OnMarkedPlayerDied);
		}
		
		HunterController->SetBTTarget(NewTarget);
	}
}

void AHunterEnemy::SetFleeing()
{
	bFleeing = true;
	RemoveHunterMarkOnPlayer();
}

bool AHunterEnemy::IsTargetMarked()
{
	if (AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(GetController()))
	{
		if (AActor* Target = HunterController->GetBTTarget())
		{
			if (ABlindEyeBaseCharacter* Player = Cast<ABlindEyePlayerCharacter>(Target))
			{
				FMarkData MarkerData = Player->GetHealthComponent()->GetCurrMark();
				if (MarkerData.bHasMark)
				{
					return MarkerData.MarkerType == EMarkerType::Hunter;
				}
			}
		}
	}
	return false;
}

bool AHunterEnemy::IsTargetOnNavigableGround()
{
	if (AHunterEnemyController* HunterController = Cast<AHunterEnemyController>(GetController()))
	{
		if (AActor* Target = HunterController->GetBTTarget())
		{
			FNavLocation ProjectedLocation;
			UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
			const FNavAgentProperties& AgentProps = GetNavAgentPropertiesRef();
			return NavSys->ProjectPointToNavigation(Target->GetActorLocation(), ProjectedLocation, INVALID_NAVEXTENT, &AgentProps);
		}
	}
	return false;
}

void AHunterEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHunterEnemy, IsVisible)
}


