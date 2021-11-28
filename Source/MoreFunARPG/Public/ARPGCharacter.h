#pragma once

#include "CoreMinimal.h"
#include "ARPGDataTableRow.h"
#include "GameFramework/Character.h"
#include "ARPGCharacter.generated.h"

DECLARE_EVENT_TwoParams(AARPGCharacter, FHealthChange, int32, int32)

UCLASS()
class MOREFUNARPG_API AARPGCharacter : public ACharacter
{
	GENERATED_BODY()

	// Constructor
public:
	AARPGCharacter();

protected:
	// Life Cycle
	virtual void BeginPlay() override;

	// Data Table
	UPROPERTY(EditDefaultsOnly)
	class UDataTable* LevelDataTable;

	TArray<struct FARPGDataTableRow*> AllLevelData;

	struct FARPGDataTableRow* CurLevelData;

	// Level Attribute
	UPROPERTY(BlueprintReadOnly)
	int32 CurExpGained;
	
	int32 MaxLevel;

	// Health Attribute
	UPROPERTY(BlueprintReadOnly)
	int32 CurHealth;

	// Operation
	void LevelUp();
	void ChangeHealth(int32 Diff);

public:
	UFUNCTION(BlueprintCallable)
	void ReceiveExp(int32 Exp);

	UFUNCTION(BlueprintCallable)
	void ReceiveDamage(int32 Damage);

	UFUNCTION(BlueprintCallable)
	void ReceiveHeal(int32 Heal);
	
	// Getter
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetCurLevel() const { return CurLevelData->Level; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetExpWorth() const { return CurLevelData->ExpWorth; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetExpNeededToNextLevel() const { return CurLevelData->ExpNeededToNextLevel; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetMaxHealth() const { return CurLevelData->MaxHealth; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetArmor() const { return CurLevelData->Armor; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetNormalDamage() const { return CurLevelData->NormalDamage; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetCriticalDamage() const { return CurLevelData->CriticalDamage; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCriticalHitRate() const { return CurLevelData->CriticalHitRate; }

	// Event / Delegate
	FORCEINLINE FHealthChange& HealthChangeEvent() { return HealthChange; }

private:
	FHealthChange HealthChange;
};
