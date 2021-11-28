#pragma once

#include "Engine/DataTable.h"
#include "ARPGDataTableRow.generated.h"

USTRUCT(BlueprintType)
struct FARPGDataTableRow : public FTableRowBase
{
	GENERATED_BODY()
	
	// Level Attribute
	UPROPERTY(EditDefaultsOnly, Category="Level")
	int32 Level = 1;

	UPROPERTY(EditDefaultsOnly, Category="Level")
	int32 ExpNeededToNextLevel = 1000;
	
	UPROPERTY(EditDefaultsOnly, Category="Level")
	int32 ExpWorth = 100; // Exp gained by other players when destroyed
	
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
