#pragma once

#include "CoreMinimal.h"
#include "ARPGCharacter.h"
#include "PlayerDataTableRow.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class MOREFUNARPG_API APlayerCharacter final : public AARPGCharacter
{
	GENERATED_BODY()

	// Constructor
public:
	APlayerCharacter();

	// Life Cycle
protected:
	virtual void Tick(const float DeltaTime) override;
	virtual void BeginPlay() override;

	// Setup Default
	void SetupComponent();
	void SetupAttachment() const;
	void SetupComponentDefaultValues() const;
	virtual void SetupDataFromDataTable() override;
	void SetupComboDefaultValues();
	virtual void SetupStateDefaultValues() override;

	// Action
	UFUNCTION(BlueprintCallable)
	bool BeginPrimaryAttack(int32& OutComboAnimIndex);

	UFUNCTION(BlueprintCallable)
	void EndPrimaryAttack();

	UFUNCTION(BlueprintCallable)
	bool BeginHealing();

	UFUNCTION(BlueprintCallable)
	void EndHealing();

	UFUNCTION(BlueprintCallable)
	bool BeginRolling();

	UFUNCTION(BlueprintCallable)
	void EndRolling();

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

	UFUNCTION(BlueprintCallable)
	void OnAttackEnabled(class UPrimitiveComponent* WeaponHitBox);

	UFUNCTION(BlueprintCallable)
	void OnAttackDisabled(class UPrimitiveComponent* WeaponHitBox);

	// BlueprintImplementableEvent
	UFUNCTION(BlueprintImplementableEvent)
	void OnInvincibleBegin();

	UFUNCTION(BlueprintImplementableEvent)
	void OnInvincibleEnd();

	UFUNCTION(BlueprintImplementableEvent)
	void OnHit();
	
	// Axis
	UFUNCTION(BlueprintCallable)
	bool MoveRight(const float Value);

	UFUNCTION(BlueprintCallable)
	bool MoveForward(const float Value);

	UFUNCTION(BlueprintCallable)
	bool Turn(const float Value);

	UFUNCTION(BlueprintCallable)
	bool LookUp(const float Value);
	
	// Attribute
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attribute")
	float MaxEnergy = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float RunningEnergyCost = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float RunningEnergyRefuel = 30.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	TArray<int32> ComboDamageList = TArray<int32>{100, 100, 100, 100};

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float ComboResetDelayTime = 0.8f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float WalkSpeed = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float RunningSpeed = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float SwitchRunningTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float DefaultInvisibleTime = 2.0f;
	
	// Runtime Member
	UPROPERTY(BlueprintReadOnly)
	float CurEnergy;

	int32 MaxCombo;
	int32 CurCombo;
	FTimerHandle ComboResetTimerHandle;

	FTimerHandle InvincibleTimerHandle;

	float TargetMovingSpeed;
	float LerpTime;

	// Data From DataTable
	TArray<struct FPlayerDataTableRow*> AllLevelData;
	struct FPlayerDataTableRow* CurLevelData;

	UPROPERTY(BlueprintReadOnly)
	int32 CurExpGained;
	
	// States
	bool bIsAttacking;
	bool bIsRolling;
	bool bIsHealing;
	bool bIsInvincible;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsMaxRunningSpeed;

	bool bIsOnHit;

	bool bIsRunning;

	FORCEINLINE bool CanMove() const { return (bIsAttacking || bIsHealing || bIsRolling || bIsOnHit) == false; };

	// Getter
	UFUNCTION(BlueprintCallable)
 	virtual int32 GetMaxHealth() const override { return CurLevelData->MaxHealth; }

	virtual int32 GetDamage() const override;

	UFUNCTION(BlueprintCallable)
	int32 GetArmor() const { return CurLevelData->Armor; }
	int32 GetNormalDamage() const { return CurLevelData->NormalDamage; }
	int32 GetCriticalDamage() const { return CurLevelData->CriticalDamage; }
	float GetCriticalHitRate() const { return CurLevelData->CriticalHitRate; }

	UFUNCTION(BlueprintCallable)
	int32 GetExpNeededToNextLevel() const { return CurLevelData->ExpNeededToNextLevel; }

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FVector GetCameraWorldLocation() const { return FollowCamera->GetComponentLocation(); }

protected:
	// Component
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* CharacterMesh;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* FollowCamera;
	
	// Operation
	void LevelUp();
public:
	UFUNCTION(BlueprintCallable)
	virtual void ReceiveDamage(int32 Damage) override;

protected:
	UFUNCTION(BlueprintCallable)
	void ReceiveExp(int32 Exp);
	
	void LerpSpeed(const float DeltaTime);
	void UpdateEnergy(const float DeltaTime);
	void InterruptExistingStates();

	FORCEINLINE void ResetCombo() { CurCombo = 0; }

	FORCEINLINE void NextCombo() { CurCombo = (CurCombo + 1) % MaxCombo; }
	
	float HorizontalInput;
	float VerticalInput;

	FORCEINLINE bool IsGettingMovementInput() const
	{
		return FMath::IsNearlyZero(HorizontalInput) == false
			|| FMath::IsNearlyZero(VerticalInput) == false;
	}
};
