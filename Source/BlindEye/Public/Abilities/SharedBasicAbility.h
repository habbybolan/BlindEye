// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AbilityBase.h"
#include "SharedBasicAbility.generated.h"

class BLINDEYE_API UFirstAttackState : public FAbilityState
{
public:
	UFirstAttackState(AAbilityBase* ability);
	virtual bool TryEnterState(bool bInputUsed) override;
	virtual void RunState() override;
	virtual void ExitState() override;
};

class BLINDEYE_API USecondAttackState : public FAbilityState
{
public:
	USecondAttackState(AAbilityBase* ability);
	virtual bool TryEnterState(bool bInputUsed) override;
	virtual void RunState() override;
	virtual void ExitState() override;
};

class BLINDEYE_API ULastAttackState : public FAbilityState
{
public:
	ULastAttackState(AAbilityBase* ability);
	virtual bool TryEnterState(bool bInputUsed) override;
	virtual void RunState() override;
	virtual void ExitState() override;
};

/**
 * 
 */
UCLASS()
class BLINDEYE_API ASharedBasicAbility : public AAbilityBase
{
	GENERATED_BODY()

public:
	ASharedBasicAbility();
	
protected:
	bool AbilityTest = true;
};




