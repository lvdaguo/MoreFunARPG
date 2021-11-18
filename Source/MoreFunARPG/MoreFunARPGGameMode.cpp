// Copyright Epic Games, Inc. All Rights Reserved.

#include "MoreFunARPGGameMode.h"
#include "MoreFunARPGCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMoreFunARPGGameMode::AMoreFunARPGGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
