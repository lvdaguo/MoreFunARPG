#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ARPGCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FHealthChange, int32, int32);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDamageReceived, int32, Damage);

// Self Define Macro
#define SUCCESS true
#define FAIL false
#define UNIMPLEMENTED 0

UCLASS()
class MOREFUNARPG_API AARPGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Constructor
	AARPGCharacter();

protected:
	// Setup Default
	virtual void SetupState() { bIsDead = false; }
	virtual void SetupDelegate();

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

	// Damage Text
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> DamageTextClass;

	UPROPERTY(EditDefaultsOnly)
	bool CanShowDamageText;

	// Safe Base
	void ChangeHealthBase(int32 Diff);
	void DealDamageBase(AARPGCharacter* Receiver) const;
	static bool BeginActionBase(bool& State, bool FailCondition);
	static void EndActionBase(bool& State);
	
	// Virtual Base
	virtual void Die();
	virtual void ReceiveDamage(int32 Damage);
	virtual int32 GetCalculatedDamage() const { return UNIMPLEMENTED; }

	// Listener
	UFUNCTION()
	virtual void OnHealthChange(const int32 Before, const int32 After);

	virtual void OnWeaponOverlap(AActor* OtherActor);

public:
	// Getter
	virtual int32 GetMaxHealth() const { return UNIMPLEMENTED; }

protected:
	// Delegate
	FHealthChange HealthChange;

	UPROPERTY(BlueprintAssignable)
	FDamageReceived DamageReceived;
	
public:
	FORCEINLINE FHealthChange& HealthChangeEvent() { return HealthChange; }
};
