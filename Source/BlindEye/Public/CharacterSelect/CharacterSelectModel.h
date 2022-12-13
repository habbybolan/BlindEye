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
class BLINDEYE_API ACharacterSelectModel : public ACharacter
{
	GENERATED_BODY()
	
public:	
	ACharacterSelectModel();

	UPROPERTY(Replicated, ReplicatedUsing="OnRep_bIsReady", BlueprintReadOnly)
	bool bIsReady = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsSelected = false;

	UPROPERTY(EditDefaultsOnly)
	UCharacterSelectBox* BoxSelection;

	UPROPERTY(EditDefaultsOnly)
	EPlayerType PlayerType;

	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* CharacterSelectWidgetComponent;

	void SelectCharacter(FString PlayerName);

	void UnSelectCharacter();

	void SetIsSelectedPlayerReady(bool IsReady);

protected:

	FCharacterSelectedInfo CharacterSelectInfo;

	UFUNCTION()
	void OnRep_bIsReady();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
