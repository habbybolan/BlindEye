// Copyright (C) Nicholas Johnson 2022


#include "Animations/AN_SnapperTutorialTrigger.h"

#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"

void UAN_SnapperTutorialTrigger::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	UWorld* World = GetWorld();
	if (World)
	{
		ABlindEyeGameState* BlindEyeGS = Cast<ABlindEyeGameState>(UGameplayStatics::GetGameState(World));
		if (BlindEyeGS)
		{
			if (BlindEyeGS->CurrEnemyTutorial == EEnemyTutorialType::BurrowerSnapper)
			{
				BlindEyeGS->EnemyTutorialTrigger(EEnemyTutorialType::BurrowerSnapper);
			}
		}
	}
}
