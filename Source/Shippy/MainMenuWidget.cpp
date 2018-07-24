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
	if (JoinAddressEdit == nullptr)
		return false;

	HostButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnHostClicked);
	JoinButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinClicked);
	return true;
}

void UMainMenuWidget::OnHostClicked()
{
}

void UMainMenuWidget::OnJoinClicked()
{
}
