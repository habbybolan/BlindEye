// Copyright (C) Nicholas Johnson 2022


#include "Animations/AN_HunterTutorialTrigger.h"

#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"

void UAN_HunterTutorialTrigger::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	UWorld* World = MeshComp->GetWorld();
	if (World)
	{
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
		if (BlindEyeGS)
		{
			if (BlindEyeGS->CurrEnemyTutorial == EEnemyTutorialType::Hunter)
			{
				BlindEyeGS->SER_EnemyTutorialTrigger(EEnemyTutorialType::Hunter);
			}
		}
	}
}
