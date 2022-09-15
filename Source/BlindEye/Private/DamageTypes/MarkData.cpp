// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/MarkData.h"
#include "BlindEyeUtils.h"

FMarkData::FMarkData()
{
	MarkPlayerType = PlayerType::CrowPlayer;
}

void FMarkData::InitializeData(PlayerType markPlayerType)
{
	this->MarkPlayerType = markPlayerType;
}