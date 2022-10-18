// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/MarkData.h"
#include "BlindEyeUtils.h"

enum class EMarkerType : uint8;

void FMarkData::InitializeData(EMarkerType markerType)
{
	this->MarkerType = markerType;
}