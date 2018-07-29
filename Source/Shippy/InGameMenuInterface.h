// Copyright (C) 2018 Robert A. Wallis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SHIPPY_API IInGameMenuInterface
{
public:
	virtual void InGameMenuExitToMainMenu() = 0;
	virtual void InGameMenuCancel() = 0;
};
