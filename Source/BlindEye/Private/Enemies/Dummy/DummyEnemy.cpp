// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Dummy/DummyEnemy.h"

#include "Enemies/Dummy/DummyEnemyHealthComponent.h"

ADummyEnemy::ADummyEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDummyEnemyHealthComponent>(TEXT("HealthComponent")))
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;
}

void ADummyEnemy::BeginPlay()
{
	Super::BeginPlay();
}
