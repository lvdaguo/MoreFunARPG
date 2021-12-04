#pragma once

#include "Engine/DataTable.h"
#include "MonsterDataTableRow.generated.h"

USTRUCT(BlueprintType)
struct FMonsterDataTableRow : public FTableRowBase
{
	GENERATED_BODY()
	
	// Level Attribute
	UPROPERTY(EditDefaultsOnly, Category="Level")
	int32 ExpWorth = 100; // Exp gained by other players when destroyed
	
	// Health Attribute
	UPROPERTY(EditDefaultsOnly, Category="Health")
	int32 MaxHealth = 100;
	
	// Battle Attribute
	UPROPERTY(EditDefaultsOnly, Category="Battle")
	int32 Damage = 20;
};
