// Copyright (C) Nicholas Johnson 2022


#include "CharacterSelect/CharacterSelectGameState.h"

#include "CharacterSelect/CharacterSelectPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/BlindEyeGameInstance.h"
#include "Gameplay/BlindEyeGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ACharacterSelectGameState::OnRep_PhoenixPlayerSelected()
{
	PlayerSelected(EPlayerType::PhoenixPlayer, PhoenixPlayer);
}

void ACharacterSelectGameState::OnRep_CrowPlayerSelected()
{
	PlayerSelected(EPlayerType::CrowPlayer, CrowPlayer);
}

void ACharacterSelectGameState::PlayerSelected(EPlayerType PlayerType, APlayerState* PlayerThatSelected)
{ 
	ACharacterSelectModel* CharacterSelectModelSelected = PlayerType == EPlayerType::CrowPlayer ? CrowModel : PhoenixModel;
	if (PlayerThatSelected == nullptr)
	{
		CharacterSelectModelSelected->UnSelectCharacter();
	} else
	{
		CharacterSelectModelSelected->SelectCharacter(PlayerThatSelected->GetPlayerName());
	}
}

void ACharacterSelectGameState::PlayerReadyStateChanged(EPlayerType PlayerType, bool bReady)
{
	ACharacterSelectModel* CharacterSelectModelSelected = PlayerType == EPlayerType::CrowPlayer ? CrowModel : PhoenixModel;
	CharacterSelectModelSelected->SetIsSelectedPlayerReady(bReady);
}

ACharacterSelectPlayerController* ACharacterSelectGameState::GetOwnerPlayerController()
{
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			return Cast<ACharacterSelectPlayerController>(PlayerController);
		}
	}
	return nullptr;
}

void ACharacterSelectGameState::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		// Cache both player models
		TArray<AActor*> OutCSModels;
		UGameplayStatics::GetAllActorsOfClass(World, ACharacterSelectModel::StaticClass(), OutCSModels);
		for (AActor* CSModelActor : OutCSModels)
		{
			ACharacterSelectModel* CSModel = Cast<ACharacterSelectModel>(CSModelActor);
			if (CSModel->PlayerType == EPlayerType::CrowPlayer)
			{
				CrowModel = CSModel;
			} else
			{
				PhoenixModel = CSModel;
			}
		}
	}
}

void ACharacterSelectGameState::PlayerTrySelect(EPlayerType PlayerType, ACharacterSelectPlayerController* ControllerThatSelected)
{
	// Update the character the has selected/unselected current character
	if (PlayerType == EPlayerType::CrowPlayer)
	{
		TrySelectHelper(PlayerType, ControllerThatSelected->PlayerState);
	} else
	{
		TrySelectHelper(PlayerType, ControllerThatSelected->PlayerState);
	}
}

void ACharacterSelectGameState::TrySelectHelper(EPlayerType PlayerType, APlayerState* PlayerThatActioned)
{
	if (PlayerThatActioned == nullptr) return;
	ACharacterSelectPlayerState** PlayerReferenceToSelectedCharacter = PlayerType == EPlayerType::CrowPlayer ? &CrowPlayer : &PhoenixPlayer;
	// Unselecting character
	if (*PlayerReferenceToSelectedCharacter == PlayerThatActioned)
	{
		// unready player if readied
		ACharacterSelectPlayerState* CharacterSelectPS = Cast<ACharacterSelectPlayerState>(PlayerThatActioned);
		if (CharacterSelectPS->GetIsReady())
		{
			CharacterSelectPS->FlipReadyState();
			PlayerReadyStateChanged(PlayerType, false);
		}
		
		*PlayerReferenceToSelectedCharacter = nullptr;
		PlayerSelected(PlayerType, nullptr);
	}
	// Selecting character
	else if (*PlayerReferenceToSelectedCharacter == nullptr && !IsPlayerSelectedCharacter(PlayerThatActioned))
	{
		*PlayerReferenceToSelectedCharacter = Cast<ACharacterSelectPlayerState>(PlayerThatActioned);
		PlayerSelected(PlayerType, *PlayerReferenceToSelectedCharacter);
	}
}

void ACharacterSelectGameState::SER_PlayerTryReady_Implementation(ACharacterSelectPlayerController* PlayerReadied)
{
	bool bPlayerReadied = false;
	// Ready up player if valid
	if (PlayerReadied->PlayerState)
	{
		if (IsPlayerSelectedCharacter(PlayerReadied->PlayerState))
		{
			ACharacterSelectPlayerState* CharacterSelectPS = Cast<ACharacterSelectPlayerState>(PlayerReadied->PlayerState);
			// Check if valid action
			if (PhoenixPlayer == CharacterSelectPS || CrowPlayer == CharacterSelectPS)
			{
				EPlayerType SelectedCharacter = PhoenixPlayer == CharacterSelectPS ? EPlayerType::PhoenixPlayer : EPlayerType::CrowPlayer;
				CharacterSelectPS->FlipReadyState();
				bPlayerReadied = CharacterSelectPS->GetIsReady();
				PlayerReadyStateChanged(SelectedCharacter, CharacterSelectPS->GetIsReady());
			}
		}
	}

	if (bPlayerReadied)
	{
		if (IsAllPlayersReady())
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Green,
				TEXT("[ACharacterSelectGameState::PlayerTryReady] CrowID: " + CrowPlayer->GetUniqueId().ToString() +
					" PhoenixID: " + PhoenixPlayer->GetUniqueId().ToString()));

			// Delay entering game
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().SetTimer(EnterGameDelayTimerHandle, this, &ACharacterSelectGameState::EnterGame, DelayToEnterGame, false);
			}
			// fade into loading screen
			MULT_StartEnterGameFade();
		}
		else
		{
			// TODO: Notify Players that player is waiting/ready
		}
	}
}

void ACharacterSelectGameState::MULT_StartEnterGameFade_Implementation()
{
	UBlindEyeGameInstance* BlindEyeGI = Cast<UBlindEyeGameInstance>(GetGameInstance());
	BlindEyeGI->CharacterSelectFadeIntoBlack(DelayToEnterGame);
}

void ACharacterSelectGameState::EnterGame()
{
	MULT_JoiningGame();
	UBlindEyeGameInstance* BlindEyeGI = Cast<UBlindEyeGameInstance>(GetGameInstance());
	BlindEyeGI->EnterGame(CrowPlayer->GetUniqueId().ToString(), PhoenixPlayer->GetUniqueId().ToString());
}

void ACharacterSelectGameState::MULT_JoiningGame_Implementation()
{
	UBlindEyeGameInstance* BlindEyeGI = Cast<UBlindEyeGameInstance>(GetGameInstance());
	BlindEyeGI->AddLoadingScreen();
}

bool ACharacterSelectGameState::IsAllPlayersReady()
{
	return CrowPlayer != nullptr && CrowPlayer->GetIsReady() &&
		PhoenixPlayer != nullptr && PhoenixPlayer->GetIsReady();
}

void ACharacterSelectGameState::OnPlayerChanged(bool bJoined, AController* ChangedController)
{
	MULT_OnPlayerChanged(bJoined, ChangedController);
}

void ACharacterSelectGameState::MULT_OnPlayerChanged_Implementation(bool bJoined, AController* ChangedController)
{
	if (GetOwnerPlayerController() == ChangedController) return;
	UBlindEyeGameInstance* BlindEyeGI = Cast<UBlindEyeGameInstance>(GetGameInstance());
	BlindEyeGI->OnPlayerChanged(bJoined);
}

bool ACharacterSelectGameState::IsPlayerSelectedCharacter(APlayerState* Player)
{
	return CrowPlayer == Player || PhoenixPlayer == Player;
}

void ACharacterSelectGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterSelectGameState, PhoenixPlayer)
	DOREPLIFETIME(ACharacterSelectGameState, CrowPlayer)
}
