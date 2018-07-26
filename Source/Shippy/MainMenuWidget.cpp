// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"

bool UMainMenuWidget::Initialize()
{
	if (!Super::Initialize())
		return false; // This is "super" important, CreateWidget<UMainMenuWidget>() will fail if this doesn't return false here.


	if (HostButton == nullptr)
		return false;
	if (JoinButton == nullptr)
		return false;
	//if (AddressTextBox == nullptr)
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

	//if (AddressTextBox == nullptr) {
	//	UE_LOG(LogTemp, Warning, TEXT("UMainMenuWidget::OnJoinClicked() AddressTextBox is null"));
	//	return;
	//}

	//auto address = AddressTextBox->Text.ToString();
	//MenuInterface->MainMenuJoinGame(address);
}
