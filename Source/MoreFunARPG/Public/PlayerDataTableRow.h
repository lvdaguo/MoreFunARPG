#pragma once

#include "Engine/DataTable.h"
#include "PlayerDataTableRow.generated.h"

USTRUCT(BlueprintType)
struct FPlayerDataTableRow : public FTableRowBase
{
	GENERATED_BODY()
	
	// Level Attribute
	UPROPERTY(EditDefaultsOnly, Category="Level")
	int32 ExpNeededToNextLevel = 1000;
	
	// Health Attribute
	UPROPERTY(EditDefaultsOnly, Category="Health")
	int32 MaxHealth = 100;

	UPROPERTY(EditDefaultsOnly,  Category="Health")
	int32 Armor = 0;
	
	// Battle Attribute
	UPROPERTY(EditDefaultsOnly, Category="Battle")
	int32 NormalDamage = 20;
	
	UPROPERTY(EditDefaultsOnly, Category="Battle")
	int32 CriticalDamage = 90;
	
	UPROPERTY(EditDefaultsOnly, Category="Battle")
	float CriticalHitRate = 0.15f;
};
