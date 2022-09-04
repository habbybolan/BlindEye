// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/MarkData.h"

#include "Characters/BlindEyeCharacter.h"

FMarkData::FMarkData()
{
	MarkPlayerType = PlayerType::CrowPlayer;
	UniqueAbilityMarkerIndex = 0;
}

void FMarkData::InitializeData(PlayerType markPlayerType, uint8 uniqueAbilityMarkerIndex)
{
	this->MarkPlayerType = markPlayerType;
	this->UniqueAbilityMarkerIndex = uniqueAbilityMarkerIndex;
}