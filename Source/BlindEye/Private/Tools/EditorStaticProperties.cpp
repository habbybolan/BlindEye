// Copyright (C) Nicholas Johnson 2022


#include "Tools/EditorStaticProperties.h"

bool UEditorStaticProperties::GetIsAssetImportScript()
{
	return IsAssetImportScript;
}

void UEditorStaticProperties::SetIsAssetImportScript(bool isAssetImportScript)
{
	IsAssetImportScript = isAssetImportScript;
}

