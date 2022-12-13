// Copyright (C) Nicholas Johnson 2022


#include "Tutorial/DetonatingTutorial/DetonatingDummyEnemy.h"
#include "Tutorial/DetonatingTutorial/DetonatingDummyHealthComponent.h"

ADetonatingDummyEnemy::ADetonatingDummyEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDetonatingDummyHealthComponent>(TEXT("HealthComponent")))
{
	
}
