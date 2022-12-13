// Copyright (C) Nicholas Johnson 2022


#include "UI/LoadingIcon.h"

ULoadingIcon::ULoadingIcon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

bool ULoadingIcon::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	RenderOpacity = 0;
	return true;
}

void ULoadingIcon::PlayLoadingIcon()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(OpacityTimerHandle, this, &ULoadingIcon::ShowIconHelper, OpacityChangeDelay, true);
	}
}

void ULoadingIcon::StopLoadingIcon()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(OpacityTimerHandle, this, &ULoadingIcon::HideIconHelper, OpacityChangeDelay, true);
	}
}

void ULoadingIcon::ShowIconHelper()
{
	SetRenderOpacity(GetRenderOpacity() + (OpacityChangeDelay / OpacityChangeTime));
	if (GetRenderOpacity() >= 1)
	{
		SetRenderOpacity(1);
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(OpacityTimerHandle);
		}
	}
}

void ULoadingIcon::HideIconHelper()
{
	SetRenderOpacity(GetRenderOpacity() - (OpacityChangeDelay / OpacityChangeTime));
	if (GetRenderOpacity() <= 0)
	{
		SetRenderOpacity(0);
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(OpacityTimerHandle);
		}
	}
}
