// Copyright (C) Nicholas Johnson 2022


#include "Enemies/BlindEyeEnemybase.h"

// Sets default values
ABlindEyeEnemyBase::ABlindEyeEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABlindEyeEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlindEyeEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABlindEyeEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

