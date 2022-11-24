// Copyright (C) Nicholas Johnson 2022

#pragma once

#include "CoreMinimal.h"
#include "CharacterSelectBox.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "UI/CharacterSelectModule.h"
#include "UI/CharacterSelectWidget.h"
#include "CharacterSelectModel.generated.h"

USTRUCT()
struct FCharacterSelectedInfo
{
	GENERATED_BODY()

	void SetAsSelected(FString playerName)
	{
		bIsSelected = true;
		PlayerName = playerName;
	}

	void SetAsUnselected()
	{
		bIsSelected = false;
	}

	UPROPERTY()
	bool bIsSelected = false;
	FString PlayerName;
};

UCLASS()
class BLINDEYE_API ACharacterSelectModel : public APawn
{
	GENERATED_BODY()
	
public:	
	ACharacterSelectModel();

	UPROPERTY(BlueprintReadOnly)
	bool bIsReady = false;

	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly)
	UCharacterSelectBox* BoxSelection;

	UPROPERTY(EditDefaultsOnly)
	EPlayerType PlayerType;

	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* CharacterSelectWidgetComponent;

	void SelectCharacter(FString PlayerName);

	void UnSelectCharacter();

protected:

	FCharacterSelectedInfo CharacterSelectInfo;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
