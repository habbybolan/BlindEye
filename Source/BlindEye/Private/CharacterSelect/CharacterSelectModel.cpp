// Copyright (C) Nicholas Johnson 2022


#include "CharacterSelect/CharacterSelectModel.h"

ACharacterSelectModel::ACharacterSelectModel()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	RootComponent = Mesh;

	BoxSelection = CreateDefaultSubobject<UCharacterSelectBox>("BoxSelect");
	BoxSelection->SetupAttachment(Mesh);

	CharacterSelectWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("CharacterSelectWidget");
	CharacterSelectWidgetComponent->SetWidgetClass(UCharacterSelectWidget::StaticClass());
	CharacterSelectWidgetComponent->SetupAttachment(BoxSelection);
}

void ACharacterSelectModel::SelectCharacter(FString PlayerName)
{
	CharacterSelectInfo.SetAsSelected(PlayerName);
	if (CharacterSelectWidgetComponent->GetWidget())
	{
		UCharacterSelectWidget* CSWidget = Cast<UCharacterSelectWidget>(CharacterSelectWidgetComponent->GetWidget());
		CSWidget->UpdateInfo(CharacterSelectInfo);
	}
}

void ACharacterSelectModel::UnSelectCharacter()
{
	CharacterSelectInfo.SetAsUnselected();
	if (CharacterSelectWidgetComponent->GetWidget())
	{
		UCharacterSelectWidget* CSWidget = Cast<UCharacterSelectWidget>(CharacterSelectWidgetComponent->GetWidget());
		CSWidget->UpdateInfo(CharacterSelectInfo);
	}
}

void ACharacterSelectModel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

