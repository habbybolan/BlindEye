// Copyright (C) Nicholas Johnson 2022


#include "DamageTypes/MarkData.h"
#include "BlindEyeUtils.h"

enum class EMarkerType : uint8;


void FMarkData::SetMark(EMarkerType NewMarkerType)
{
	bHasMark = true;
	MarkerType = NewMarkerType;
}

void FMarkData::RemoveMark()
{
	bHasMark = false;
}
