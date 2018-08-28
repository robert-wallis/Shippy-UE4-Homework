// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#include "MainMenu.h"

#include "UObject/ConstructorHelpers.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/PanelWidget.h"

#include "ServerRow.h"
#include "../LogShippy.h"

UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/Menu/ServerRow_WBP"));
	ServerRowClass = ServerRowBPClass.Class;
	if (ServerRowClass == nullptr) {
		UE_LOG(LogShippy, Error, TEXT("UMainMenu Couldn't find ServerRow_WBP for ServerRowClass"));
	}
}

bool UMainMenu::Initialize()
{
	if (!Super::Initialize())
		return false; // This is "super" important, CreateWidget<UMainMenu>() will fail if this doesn't return false here.

	if (HostButton == nullptr)
		return false;
	if (SearchButton == nullptr)
		return false;
	if (SearchList == nullptr)
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

void UMainMenu::SearchClearResults()
{
	SearchList->ClearChildren();
}

void UMainMenu::SearchAddServer(const FLobbyServer& Server, const int SearchIndex)
{
	auto Row = CreateWidget<UServerRow>(GetWorld(), ServerRowClass);
	SearchList->AddChild(Row);
	Row->Init(this, Server, SearchIndex);
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
	Interface->MainMenuServerRefresh();
}

void UMainMenu::OnSearchBackClicked()
{
	Switcher->SetActiveWidget(MainMenu);
}

void UMainMenu::OnJoinClicked()
{
	if (Interface == nullptr) {
		UE_LOG(LogShippy, Warning, TEXT("UMainMenu::OnJoinClicked() MainMenuInterface is null"));
		return;
	}

	if (AddressTextBox == nullptr)
		return;

	auto Address = AddressTextBox->Text.ToString();

	if (Address.Len() <= 0) {
		return;
	}

	UE_LOG(LogShippy, Log, TEXT("UMainMenu::OnJoinClicked: %s"), *Address);
	Interface->MainMenuJoinGame(Address);
}

void UMainMenu::OnCreditsClicked()
{
	if (CreditsMenu == nullptr)
		return;
	if (Switcher == nullptr)
		return;

	Switcher->SetActiveWidget(CreditsMenu);
}

void UMainMenu::ServerRowJoin(const int SearchIndex)
{
	Interface->MainMenuJoinServer(SearchIndex);
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

