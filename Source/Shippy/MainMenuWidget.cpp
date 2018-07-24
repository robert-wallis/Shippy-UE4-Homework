// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"

bool UMainMenuWidget::Initialize()
{
	if (HostButton == nullptr)
		return false;
	if (JoinButton == nullptr)
		return false;
	//if (JoinAddressEdit == nullptr)
	//	return false;

	HostButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnHostClicked);
	JoinButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinClicked);
	return true;
}

void UMainMenuWidget::SetMenuInterface(IMainMenuInterface * mainMenuInterface)
{
	MenuInterface = mainMenuInterface;
}

void UMainMenuWidget::OnHostClicked()
{
	if (MenuInterface == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("UMainMenuWidget::OnHostClicked() MenuInterface is null"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("UMainMenuWidget::OnHostClicked() Host command sent"));

	MenuInterface->MainMenuHost();
}

void UMainMenuWidget::OnJoinClicked()
{
	if (MenuInterface == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("UMainMenuWidget::OnJoinClicked() MenuInterface is null"));
		return;
	}

	//if (JoinAddressEdit == nullptr) {
	//	UE_LOG(LogTemp, Warning, TEXT("UMainMenuWidget::OnJoinClicked() JoinAddressEdit is null"));
	//	return;
	//}

	//auto address = JoinAddressEdit->Text.ToString();
	//MenuInterface->MainMenuJoinGame(address);
}
