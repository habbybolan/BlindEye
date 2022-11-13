// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/DummyEnemy.h"

#include "Tutorial/DummyEnemyHealthComponent.h"

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
