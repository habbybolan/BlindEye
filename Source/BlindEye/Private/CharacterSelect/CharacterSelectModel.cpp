// Copyright (C) Nicholas Johnson 2022


#include "CharacterSelect/CharacterSelectModel.h"

ACharacterSelectModel::ACharacterSelectModel()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	BoxSelection = CreateDefaultSubobject<UCharacterSelectBox>("BoxSelect");
	BoxSelection->SetupAttachment(GetMesh());

	CharacterSelectWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("CharacterSelectWidget");
	CharacterSelectWidgetComponent->SetWidgetClass(UCharacterSelectWidget::StaticClass());
	CharacterSelectWidgetComponent->SetupAttachment(GetMesh());
}

void ACharacterSelectModel::SelectCharacter(FString PlayerName)
{
	CharacterSelectInfo.SetAsSelected(PlayerName);
	if (CharacterSelectWidgetComponent->GetWidget())
	{
		UCharacterSelectWidget* CSWidget = Cast<UCharacterSelectWidget>(CharacterSelectWidgetComponent->GetWidget());
		CSWidget->UpdateInfo(CharacterSelectInfo);
		bIsReady = true;
	}
}

void ACharacterSelectModel::UnSelectCharacter()
{
	CharacterSelectInfo.SetAsUnselected();
	if (CharacterSelectWidgetComponent->GetWidget())
	{
		UCharacterSelectWidget* CSWidget = Cast<UCharacterSelectWidget>(CharacterSelectWidgetComponent->GetWidget());
		CSWidget->UpdateInfo(CharacterSelectInfo);
		bIsReady = false;
	}
}

void ACharacterSelectModel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

