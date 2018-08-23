// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#include "MainMenu.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/WidgetSwitcher.h"
#include "../LogShippy.h"

bool UMainMenu::Initialize()
{
	if (!Super::Initialize())
		return false; // This is "super" important, CreateWidget<UMainMenu>() will fail if this doesn't return false here.

	if (HostButton == nullptr)
		return false;
	if (JoinButton == nullptr)
		return false;
	if (AddressTextBox == nullptr)
		return false;
	if (QuitButton == nullptr)
		return false;
	if (Switcher == nullptr)
		return false;
	if (MainMenu == nullptr)
		return false;
	if (CreditsMenu == nullptr)
		return false;
	if (CreditsButton == nullptr)
		return false;
	if (CreditsBackButton == nullptr)
		return false;

	HostButton->OnClicked.AddDynamic(this, &UMainMenu::OnHostClicked);
	SearchButton->OnClicked.AddDynamic(this, &UMainMenu::OnSearchClicked);
	SearchBackButton->OnClicked.AddDynamic(this, &UMainMenu::OnSearchBackClicked);
	SearchJoinButton->OnClicked.AddDynamic(this, &UMainMenu::OnSearchJoinClicked);
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OnJoinClicked);
	CreditsButton->OnClicked.AddDynamic(this, &UMainMenu::OnCreditsClicked);
	QuitButton->OnClicked.AddDynamic(this, &UMainMenu::OnQuitClicked);
	CreditsBackButton->OnClicked.AddDynamic(this, &UMainMenu::OnBackToMainMenu);
	return true;
}

void UMainMenu::SetInterface(MainMenuInterface * Interface)
{
	this->Interface = Interface;
}

void UMainMenu::OnHostClicked()
{
	if (Interface == nullptr) {
		UE_LOG(LogShippy, Warning, TEXT("UMainMenu::OnHostClicked() MainMenuInterface is null"));
		return;
	}

	Interface->MainMenuHost();
}

void UMainMenu::OnSearchClicked()
{
	Switcher->SetActiveWidget(SearchMenu);
}

void UMainMenu::OnSearchBackClicked()
{
	Switcher->SetActiveWidget(MainMenu);
}

void UMainMenu::OnSearchJoinClicked()
{
	UE_LOG(LogShippy, Warning, TEXT("TODO:  UMainMenu::OnSearchJoinClicked Join Selected Server"));
}

void UMainMenu::OnJoinClicked()
{
	if (Interface == nullptr) {
		UE_LOG(LogShippy, Warning, TEXT("UMainMenu::OnJoinClicked() MainMenuInterface is null"));
		return;
	}

	if (AddressTextBox == nullptr)
		return;

	auto address = AddressTextBox->Text.ToString();

	if (address.Len() <= 0) {
		return;
	}

	Interface->MainMenuJoinGame(address);
}

void UMainMenu::OnCreditsClicked()
{
	if (CreditsMenu == nullptr)
		return;
	if (Switcher == nullptr)
		return;

	Switcher->SetActiveWidget(CreditsMenu);
}

void UMainMenu::OnQuitClicked()
{
	if (Interface == nullptr)
		return;

	Interface->MainMenuQuit();
}

void UMainMenu::OnBackToMainMenu()
{
	if (MainMenu == nullptr)
		return;
	if (Switcher == nullptr)
		return;

	Switcher->SetActiveWidget(MainMenu);
}

