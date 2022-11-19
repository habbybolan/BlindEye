// Copyright (C) Nicholas Johnson 2022


#include "UI/SessionRow.h"

void USessionRow::SetSelected(bool IsSelected)
{
	bIsSelected = true;
	BP_SetSelected(IsSelected);
}
