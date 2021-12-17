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

public:
	// Constructor
	AMonsterCharacter();

protected:
	// Setup Default
	void SetupComponent();
	void SetupAttachment() const;
	void SetupComponentDefaultValues() const;

	// Setup Runtime
	virtual void SetupState() override;
	void SetupDataFromDataTable() ;
	virtual void SetupDelegate() override;
	void SetupRandomMesh();
	
	// Level Setup
	void SetupLevelByTime();
	void SetupByLevel(int32 Level);

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

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float DeadBodyExistTime = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	TArray<float> LevelUnlockTimeList = {20.0f, 40.0f, 60.0f, 80.0f};
	
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

public:
	// Getter
	FORCEINLINE int32 GetCurHealth() const { return CurHealth; }
	FORCEINLINE int32 GetMaxHealth() const { return MaxHealth; }
 	FORCEINLINE float GetLowHealthPercent() const { return LowHealthPercent; } 
	FORCEINLINE int32 GetHealthPotion() const { return HealPotion; }
	FORCEINLINE int32 GetExpWorth() const { return CurLevelData->ExpWorth; }
	FORCEINLINE int32 GetScore() const { return Score; }	

protected:
	// Behavior Tree Task Helper
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
	void OnPlayerCameraLocationUpdated(FVector PlayerCamLocation);
	
	// Delegate
	FMonsterDie MonsterDie;

public:
	FORCEINLINE FMonsterDie& MonsterDieEvent() { return MonsterDie; }
};
