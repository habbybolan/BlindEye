// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AbilityBase.h"
#include "SharedBasicAbility.generated.h"

class BLINDEYE_API UFirstAttackState : public FAbilityState
{
public:
	UFirstAttackState(UAbilityBase* ability);
	virtual bool TryEnterState(bool bInputUsed) override;
	virtual void RunState() override;
	virtual void ExitState() override;
};

class BLINDEYE_API USecondAttackState : public FAbilityState
{
public:
	USecondAttackState(UAbilityBase* ability);
	virtual bool TryEnterState(bool bInputUsed) override;
	virtual void RunState() override;
	virtual void ExitState() override;
};

class BLINDEYE_API ULastAttackState : public FAbilityState
{
public:
	ULastAttackState(UAbilityBase* ability);
	virtual bool TryEnterState(bool bInputUsed) override;
	virtual void RunState() override;
	virtual void ExitState() override;
};

/**
 * 
 */
UCLASS()
class BLINDEYE_API USharedBasicAbility : public UAbilityBase
{
	GENERATED_BODY()

public:
	USharedBasicAbility();
	
protected:
	bool AbilityTest = true;
};




