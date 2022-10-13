// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/MarkData.h"
#include "BlindEyeUtils.h"

FMarkData::FMarkData()
{
	MarkPlayerType = EPlayerType::CrowPlayer;
}

void FMarkData::InitializeData(EPlayerType markPlayerType)
{
	this->MarkPlayerType = markPlayerType;
}