#pragma once

#include "CoreMinimal.h"
#include "ARPGCharacter.h"
#include "DataTableRow/MonsterDataTableRow.h"
#include "MonsterCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FMonsterDie, const class AMonsterCharacter*);

UCLASS()
class MOREFUNARPG_API AMonsterCharacter final : public AARPGCharacter
{
	GENERATED_BODY()

	// Constructor
public:
	AMonsterCharacter();

protected:
	// Setup Default
	void SetupComponent();
	void SetupAttachment() const;
	void SetupComponentDefaultValues() const;

	// Setup Runtime
	virtual void SetupState() override;
	void SetupDataFromDataTable() ;
	void SetupDelegate();
	void RandomSetupMesh();
	
public:
	// Level Setup
	void SetupByLevel(int32 Level);

protected:
	// Life Cycle
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Component
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* HealthBarTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UWidgetComponent* HealthBar;
	
	// Data From DataTable
	TArray<struct FMonsterDataTableRow*> AllLevelData;
	struct FMonsterDataTableRow* CurLevelData;
	
	// Attribute
	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	TArray<class USkeletalMesh*> RandomMeshPool; 
	
	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float PatrolSpeed = 400.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float ChaseSpeed = 800.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float SwitchRunningTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attribute")
	float DefaultInvincibleTime = 0.8f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	int32 HealPotion = 1;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	int32 HealAmount = 50;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float LowHealthPercent = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	int32 Score = 100;
	
	float LerpTime;
	float TargetMovingSpeed;
	
	// States
	bool bIsInvincible;
	bool bIsOnHit;
	bool bIsRunning;
	bool bIsHealing;
	bool bIsAttacking;

	void InterruptExistingStates();

	// Condition
	FORCEINLINE bool CanAct() const { return (bIsAttacking || bIsHealing || bIsOnHit) == false; }

	// Getter
public:
	UFUNCTION(BlueprintCallable)
	virtual int32 GetMaxHealth() const override { return CurLevelData->MaxHealth; }
	
	FORCEINLINE int32 GetCurHealth() const { return CurHealth; }
 	FORCEINLINE float GetLowHealthPercent() const { return LowHealthPercent; } 
	FORCEINLINE int32 GetHealthPotion() const { return HealPotion; }
	FORCEINLINE int32 GetExpWorth() const { return CurLevelData->ExpWorth; }
	FORCEINLINE int32 GetScore() const { return Score; }	

	// Behavior Tree Task Helper
protected:
	UFUNCTION(BlueprintCallable)
	bool BeginAttack();

	UFUNCTION(BlueprintCallable)
	void EndAttack();

	UFUNCTION(BlueprintCallable)
	bool BeginHealing();

	UFUNCTION(BlueprintCallable)
	void EndHealing();
	
	UFUNCTION(BlueprintCallable)
	bool BeginRunning();

	UFUNCTION(BlueprintCallable)
	void EndRunning();

	UFUNCTION(BlueprintCallable)
	bool BeginOnHit();

	UFUNCTION(BlueprintCallable)
	void EndOnHit();

	UFUNCTION(BlueprintCallable)
	bool BeginInvincible();

	UFUNCTION(BlueprintCallable)
	void EndInvincible();
	
	// Blueprint Implementable
	UFUNCTION(BlueprintImplementableEvent)
	void OnHit();
	
	// Override
	UFUNCTION(BlueprintCallable)
	virtual void OnWeaponOverlap(AActor* OtherActor) override;

	UFUNCTION(BlueprintCallable)
	virtual void ReceiveDamage(int32 Damage) override;
	
	virtual void Die() override;
	FORCEINLINE virtual int32 GetCalculatedDamage() const override { return CurLevelData->Damage; }
	
	UFUNCTION(BlueprintCallable)
	void ReceiveHeal();

	// Running
	void LerpSpeed(float DeltaTime);

	// UI
	void BarFacingTarget(FVector TargetLocation) const;

	// Weapon
	UFUNCTION(BlueprintCallable)
	static void EnableWeapon(class UPrimitiveComponent* WeaponHitBox);

	UFUNCTION(BlueprintCallable)
	static void DisableWeapon(class UPrimitiveComponent* WeaponHitBox);

	// Listener
	UFUNCTION()
	void OnPlayerCamLocationUpdated(FVector PlayerCamLocation);

	// Delegate
	FMonsterDie MonsterDie;

public:
	FORCEINLINE FMonsterDie& MonsterDieEvent() { return MonsterDie; }
};
