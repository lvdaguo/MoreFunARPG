#pragma once

#include "Engine/DataTable.h"
#include "BossDataTableRow.generated.h"

USTRUCT(BlueprintType)
struct FBossDataTableRow : public FTableRowBase
{
	GENERATED_BODY()

	// Level Attribute
	UPROPERTY(EditDefaultsOnly, Category="Level")
	int32 ExpWorth = 500; // Exp gained by other players when destroyed
	
	// Health Attribute
	UPROPERTY(EditDefaultsOnly, Category="Health")
	int32 MaxHealth = 1000;
	
	// Battle Attribute
	UPROPERTY(EditDefaultsOnly, Category="Battle")
	int32 MeleeDamage = 20;

	UPROPERTY(EditDefaultsOnly, Category="Battle")
	int32 RangeDamage = 40;

	UPROPERTY(EditDefaultsOnly, Category="Battle")
	int32 ChargeDamage = 20;
};
