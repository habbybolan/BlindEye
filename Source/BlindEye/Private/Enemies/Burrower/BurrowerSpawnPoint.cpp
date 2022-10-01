// Copyright (C) Nicholas Johnson 2022


#include "Enemies/Burrower/BurrowerSpawnPoint.h"

// Sets default values
ABurrowerSpawnPoint::ABurrowerSpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	// if (ArrowComponent)
	// {
	// 	ArrowComponent->ArrowColor = FColor(150, 200, 255);
	// 	ArrowComponent->bTreatAsASprite = true;
	// 	ArrowComponent->SetupAttachment(RootComponent);
	// 	ArrowComponent->bIsScreenSizeScaled = true;
	// }
}

// Called when the game starts or when spawned
void ABurrowerSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABurrowerSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

