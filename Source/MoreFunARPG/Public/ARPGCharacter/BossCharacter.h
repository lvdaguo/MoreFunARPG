#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "ARPGCharacter/ARPGCharacter.h"
#include "DataTableRow/BossDataTableRow.h"
#include "BossCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FBossDie);

UCLASS()
class MOREFUNARPG_API ABossCharacter final : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	// Constructor
	ABossCharacter();

protected:
	// Setup Runtime
	virtual void SetupState() override;
	void SetupDataFromDataTable();

	// Level Setup
	void SetupLevelByTime();
	void SetupByLevel(int32 Level);

	// Life Cycle
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Data From DataTable
	TArray<struct FBossDataTableRow*> AllLevelData;
	struct FBossDataTableRow* CurLevelData;

	// Attribute
	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float Speed = 600.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attribute")
	float DefaultInvincibleTime = 0.8f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float LowHealthPercent = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	int32 ScoreWorth = 500;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float DeadBodyExistTime = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	TArray<float> LevelUnlockTimeList = {20.0f, 40.0f, 60.0f, 80.0f};

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	TArray<float> StunTriggerList = {0.75, 0.5, 0.25};

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float ChargeForce = 100;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float MeleeRange = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float MidRange = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float ChargeCoolDownTime = 10.0f;
	
	float LerpTime;
	float TargetMovingSpeed;

	// States
	bool bIsInvincible;
	bool bIsStunning;

	UPROPERTY(BlueprintReadOnly)
	bool bIsMeleeAttacking;

	UPROPERTY(BlueprintReadOnly)
	bool bIsRangeAttacking;

	UPROPERTY(BlueprintReadOnly)
	bool bIsCharging;

	void InterruptExistingStates();

	// Condition
	FORCEINLINE bool CanAct() const
	{
		return (bIsMeleeAttacking || bIsCharging || bIsRangeAttacking || bIsStunning) == false;
	}

public:
	// Getter
	FORCEINLINE int32 GetMeleeDamage() const { return CurLevelData->MeleeDamage; }
	FORCEINLINE int32 GetRangeDamage() const { return CurLevelData->RangeDamage; }
	FORCEINLINE int32 GetChargeDamage() const { return CurLevelData->ChargeDamage; }
	FORCEINLINE int32 GetCurHealth() const { return CurHealth; }
	FORCEINLINE float GetLowHealthPercent() const { return LowHealthPercent; }
	FORCEINLINE int32 GetExpWorth() const { return CurLevelData->ExpWorth; }
	FORCEINLINE int32 GetScoreWorth() const { return ScoreWorth; }
	FORCEINLINE float GetMeleeRange() const { return MeleeRange; }
	FORCEINLINE float GetMidRange() const { return MidRange; }
	FORCEINLINE bool IsStunning() const { return bIsStunning; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetChargeCoolDownTime() const { return ChargeCoolDownTime; }
	
protected:
	// Behavior Tree Task Helper
	UFUNCTION(BlueprintCallable)
	bool BeginMeleeAttack();

	UFUNCTION(BlueprintCallable)
	void EndMeleeAttack();

	UFUNCTION(BlueprintCallable)
	bool BeginRangeAttack();

	UFUNCTION(BlueprintCallable)
	void EndRangeAttack();

	UFUNCTION(BlueprintCallable)
	bool BeginCharge();

	UFUNCTION(BlueprintCallable)
	void EndCharge();

	UFUNCTION(BlueprintCallable)
	bool BeginStun();

	UFUNCTION(BlueprintCallable)
	void EndStun();

	UFUNCTION(BlueprintCallable)
	bool BeginInvincible();

	UFUNCTION(BlueprintCallable)
	void EndInvincible();

	// Blueprint Implementable
	UFUNCTION(BlueprintImplementableEvent)
	void OnStun();

	// Override
	UFUNCTION(BlueprintCallable)
	virtual void OnWeaponOverlap(AActor* OtherActor) override;

	UFUNCTION(BlueprintCallable)
	virtual void ReceiveDamage(int32 Damage) override;

	virtual void Die() override;
	FORCEINLINE virtual int32 GetCalculatedDamage() const override;

	// Weapon
	UFUNCTION(BlueprintCallable)
	virtual void EnableWeapon(class UPrimitiveComponent* WeaponHitBox) override;

	UFUNCTION(BlueprintCallable)
	virtual void DisableWeapon(class UPrimitiveComponent* WeaponHitBox) override;

	// Listener
	UFUNCTION(BlueprintCallable)
	void OnChargeOverlap(class AActor* OtherActor);

	virtual void OnHealthChange(const int32 Before, const int32 After) override;

	// Delegate
	FBossDie BossDie;

public:
	FORCEINLINE FBossDie& BossDieEvent() { return BossDie; }
};
