// Copyright (C) Nicholas Johnson 2022


#include "CharacterSelect/CharacterSelectModel.h"

#include "Net/UnrealNetwork.h"

ACharacterSelectModel::ACharacterSelectModel()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	BoxSelection = CreateDefaultSubobject<UCharacterSelectBox>("BoxSelect");
	BoxSelection->SetupAttachment(GetMesh());

	CharacterSelectWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("CharacterSelectWidget");
	CharacterSelectWidgetComponent->SetWidgetClass(UCharacterSelectWidget::StaticClass());
	CharacterSelectWidgetComponent->SetupAttachment(GetMesh());

	CharacterSelectWidgetComponent->SetupAttachment(GetMesh());
}

void ACharacterSelectModel::SelectCharacter(FString PlayerName)
{
	CharacterSelectInfo.SetAsSelected(PlayerName);
	if (CharacterSelectWidgetComponent->GetWidget())
	{
		UCharacterSelectWidget* CSWidget = Cast<UCharacterSelectWidget>(CharacterSelectWidgetComponent->GetWidget());
		CSWidget->UpdateInfo(CharacterSelectInfo);
		bIsSelected = true;
	}
}

void ACharacterSelectModel::UnSelectCharacter()
{
	CharacterSelectInfo.SetAsUnselected();
	if (CharacterSelectWidgetComponent->GetWidget())
	{
		UCharacterSelectWidget* CSWidget = Cast<UCharacterSelectWidget>(CharacterSelectWidgetComponent->GetWidget());
		CSWidget->UpdateInfo(CharacterSelectInfo);
		bIsSelected = false;
	}
}

void ACharacterSelectModel::SetIsSelectedPlayerReady(bool IsReady)
{
	bIsReady = IsReady;
	OnRep_bIsReady();
}

void ACharacterSelectModel::OnRep_bIsReady()
{
}

void ACharacterSelectModel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACharacterSelectModel, bIsReady);
}

