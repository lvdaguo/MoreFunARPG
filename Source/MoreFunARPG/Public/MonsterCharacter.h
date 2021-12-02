#pragma once

#include "CoreMinimal.h"
#include "ARPGCharacter.h"
#include "MonsterDataTableRow.h"
#include "MonsterCharacter.generated.h"

UCLASS()
class MOREFUNARPG_API AMonsterCharacter final : public AARPGCharacter
{
	GENERATED_BODY()

	// Constructor
public:
	void SetupComponent();
	void SetupAttachment() const;
	void SetupComponentDefaultValues() const;
	AMonsterCharacter();

	// Setup Default
	virtual void SetupDataFromDataTable() override;
	void SetupByLevel(int32 Level);
	virtual void SetupStateDefaultValues() override;
	
	// Life Cycle
protected:
	virtual void BeginPlay() override;
	void BarFacingPlayer() const;
	void LerpSpeed(float DeltaTime);
	virtual void Tick(float DeltaTime) override;

	// Attribute
	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	TArray<class USkeletalMesh*> RandomMeshPool; 
	
	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float PatrolSpeed = 400.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float ChaseSpeed = 800.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float SwitchRunningTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float DefaultInvisibleTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	int32 HealLeft = 1;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	int32 HealAmount = 50;
	
	float LerpTime;
	float TargetMovingSpeed;
	
	// States
	bool bIsInvincible;
	bool bIsOnHit;
	bool bIsRunning;
	bool bIsHealing;
	bool bIsAttacking;
	FTimerHandle InvincibleTimerHandle;

	FORCEINLINE bool CanAct() const { return (bIsAttacking || bIsHealing || bIsOnHit) == false; }
	void InterruptExistingStates();

	// Data From DataTable
	TArray<struct FMonsterDataTableRow*> AllLevelData;
	struct FMonsterDataTableRow* CurLevelData;

	// Component
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* HealthBarTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UWidgetComponent* HealthBar;

	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;

public:
	// Getter
	UFUNCTION(BlueprintCallable)
	virtual int32 GetMaxHealth() const override { return CurLevelData->MaxHealth; }

protected:
	FORCEINLINE virtual int32 GetCalculatedDamage() const override { return CurLevelData->Damage; }
	FORCEINLINE int32 GetExpWorth() const { return CurLevelData->ExpWorth; }
	
	// Action
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

	bool BeginInvincible();

	void EndInvincible();

	// Blueprint Implementable
	UFUNCTION(BlueprintImplementableEvent)
	void OnHit();
	
	// Operation
	virtual void Die() override;

	UFUNCTION(BlueprintCallable)
	void ReceiveHeal();

	UFUNCTION(BlueprintCallable)
	virtual void ReceiveDamage(int32 Damage) override;
};
