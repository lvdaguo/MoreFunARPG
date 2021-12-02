#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ARPGCharacter.generated.h"

DECLARE_EVENT_TwoParams(AARPGCharacter, FHealthChange, int32, int32)

// Self Define Macro
#define SUCCESS true
#define FAIL false
// #define CHECK_DEAD() \
// { \
// 	if (bIsDead) \
// 	{ \
// 		return FAIL; \
// 	} \
// }

UCLASS()
class MOREFUNARPG_API AARPGCharacter : public ACharacter
{
	GENERATED_BODY()

	// Constructor
public:
	AARPGCharacter();

protected:
	// Setup Default
	virtual void SetupDataFromDataTable() { }
	virtual void SetupStateDefaultValues();
	
	// Life Cycle
	virtual void BeginPlay() override;

	// Data Table
	UPROPERTY(EditDefaultsOnly)
	class UDataTable* LevelDataTable;
	
	// Level Attribute
	UPROPERTY(BlueprintReadOnly)
	int32 MaxLevel;

	// Health Attribute
	UPROPERTY(BlueprintReadOnly)
	int32 CurHealth;

	UPROPERTY(BlueprintReadOnly)
	int32 CurLevel;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDead;

	// Operation
	void ChangeHealthSafe(int32 Diff);
	void OnHealthChange(int32 Before, int32 After);
	virtual void Die();

	// Collision
	UFUNCTION(BlueprintCallable)
	void OnWeaponOverlap(AActor* OtherActor);
	
	virtual void ReceiveDamage(int32 Damage);

public:
	// Getter
	virtual int32 GetMaxHealth() const { return 0; }

protected:
	virtual int32 GetCalculatedDamage() const { return 0; }

	// Event / Delegate
	FORCEINLINE FHealthChange& HealthChangeEvent() { return HealthChange; }

private:
	FHealthChange HealthChange;
};
