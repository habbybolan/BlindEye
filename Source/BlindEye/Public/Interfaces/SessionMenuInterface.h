// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SessionMenuInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USessionMenuInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BLINDEYE_API ISessionMenuInterface
{
	GENERATED_BODY()

public:

	virtual void Host(FString ServerName) = 0;
	virtual void JoinSession(uint32 Index) = 0;
	virtual void EndSession() = 0;
	
	virtual void SetIsLAN(bool IsLan) = 0;
	virtual FString GetLobbyName() = 0;

	virtual void RefreshSessionList() = 0;
};
