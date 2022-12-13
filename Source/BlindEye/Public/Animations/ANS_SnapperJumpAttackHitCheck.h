// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Enemies/Snapper/SnapperEnemy.h"
#include "ANS_SnapperJumpAttackHitCheck.generated.h"

/**
 * 
 */
UCLASS()
class BLINDEYE_API UANS_SnapperJumpAttackHitCheck : public UAnimNotifyState
{
public:
	
	GENERATED_BODY()
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<	EObjectTypeQuery>> PlayerObjectType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float JumpAttackDamage = 5;

	UPROPERTY(EditDefaultsOnly)
	float AttackTraceBoxLength = 300;
 
	UPROPERTY(EditDefaultsOnly)
	float AttackTraceBoxWidth = 300;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UBaseDamageType> JumpAttackDamageType;
	
	UPROPERTY()
	TWeakObjectPtr<ASnapperEnemy> Snapper;
};
