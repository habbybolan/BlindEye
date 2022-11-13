// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/DetonatingDummyEnemy.h"
#include "Tutorial/DetonatingDummyHealthComponent.h"

ADetonatingDummyEnemy::ADetonatingDummyEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDetonatingDummyHealthComponent>(TEXT("HealthComponent")))
{
	
}
