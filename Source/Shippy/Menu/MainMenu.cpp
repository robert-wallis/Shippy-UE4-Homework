// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#include "MainMenu.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/WidgetSwitcher.h"

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
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OnJoinClicked);
	CreditsButton->OnClicked.AddDynamic(this, &UMainMenu::OnCreditsClicked);
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
		UE_LOG(LogTemp, Warning, TEXT("UMainMenu::OnHostClicked() MenuInterface is null"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("UMainMenu::OnHostClicked() Host command sent"));

	Interface->MainMenuHost();
}

void UMainMenu::OnJoinClicked()
{
	if (Interface == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("UMainMenu::OnJoinClicked() MenuInterface is null"));
		return;
	}

	if (AddressTextBox == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("UMainMenu::OnJoinClicked() AddressTextBox is null"));
		return;
	}

	auto address = AddressTextBox->Text.ToString();

	if (address.Len() <= 0) {
		return;
	}

	Interface->MainMenuJoinGame(address);
}

void UMainMenu::OnBackToMainMenu()
{
	if (MainMenu == nullptr)
		return;
	if (Switcher == nullptr)
		return;

	Switcher->SetActiveWidget(MainMenu);
}

void UMainMenu::OnCreditsClicked()
{
	if (CreditsMenu == nullptr)
		return;
	if (Switcher == nullptr)
		return;

	Switcher->SetActiveWidget(CreditsMenu);
}