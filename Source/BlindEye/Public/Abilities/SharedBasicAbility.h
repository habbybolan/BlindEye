// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AbilityBase.h"
#include "SharedBasicAbility.generated.h"

class BLINDEYE_API UFirstAttackState : public AbilityState
{
public:
	UFirstAttackState();
	virtual bool EnterState() override;
	virtual void RunState() override;
	virtual void ExitState() override;
};

class BLINDEYE_API USecondAttackState : public AbilityState
{
public:
	USecondAttackState();
	virtual bool EnterState() override;
	virtual void RunState() override;
	virtual void ExitState() override;
};

class BLINDEYE_API ULastAttackState : public AbilityState
{
public:
	ULastAttackState();
	virtual bool EnterState() override;
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
	UFirstAttackState* FirstAttackState; 
	USecondAttackState* SecondAttackState; 
	ULastAttackState* LastAttackState; 
};




