// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EditorStaticProperties.generated.h"

/**
 * Holder for all static properties used in editor for UI and Tools
 */
UCLASS()
class BLINDEYE_API UEditorStaticProperties : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	static bool GetIsAssetImportScript();

	UFUNCTION(BlueprintCallable)
	static void SetIsAssetImportScript(bool isAssetImportScript);
	
};

static bool IsAssetImportScript = true;
