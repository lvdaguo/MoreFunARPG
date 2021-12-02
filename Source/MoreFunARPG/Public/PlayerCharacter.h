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

	// Action Blueprint Implementation Helper
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

	UFUNCTION(BlueprintCallable)
	bool BeginInvincible();

	UFUNCTION(BlueprintCallable)
	void EndInvincible();

	UFUNCTION(BlueprintCallable)
	void EnableWeapon(class UPrimitiveComponent* WeaponHitBox);

	UFUNCTION(BlueprintCallable)
	void DisableWeapon(class UPrimitiveComponent* WeaponHitBox);

	// BlueprintImplementableEvent
	UFUNCTION(BlueprintImplementableEvent)
	void OnInvincibleBegin();

	UFUNCTION(BlueprintImplementableEvent)
	void OnInvincibleEnd();

	UFUNCTION(BlueprintImplementableEvent)
	void OnHit();

	// Action
	// implemented in PlayerCharacter blueprint
	// bind in PlayerController cpp script 
public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnPrimaryAttackPressed();

	UFUNCTION(BlueprintImplementableEvent)
	void OnHealPressed();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRollPressed();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRunningPressed();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRunningReleased();

	// Axis
	void MoveRight(const float Value);
	void MoveForward(const float Value);
	void Turn(const float Value);
	void LookUp(const float Value);

protected:
	// Attribute
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attribute")
	float MaxEnergy = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float RunningEnergyCost = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float RunningEnergyRefuel = 20.0f;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attribute")
	float DefaultInvincibleTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float HalfRunningAngle = 60.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float RollEnergyCost = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	int32 HealPotion = 1;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	int32 HealAmount = 50;
	
	// Runtime Member
	UPROPERTY(BlueprintReadOnly)
	float CurEnergy;

	int32 MaxCombo;
	int32 CurCombo;
	FTimerHandle ComboResetTimerHandle;

	FTimerHandle InvincibleTimerHandle;

	float TargetMovingSpeed;
	float LerpTime;

	float VerticalInput;

	// Data From DataTable
	TArray<struct FPlayerDataTableRow*> AllLevelData;
	struct FPlayerDataTableRow* CurLevelData;

	UPROPERTY(BlueprintReadOnly)
	int32 CurExpGained;

	// States
	bool bIsAttacking;
	bool bIsComboActive;
	bool bIsRolling;
	bool bIsHealing;
	bool bIsInvincible;

	UPROPERTY(BlueprintReadOnly)
	bool bIsMaxRunningSpeed;

	bool bIsOnHit;

	bool bIsRunning;

	// Condition
	FORCEINLINE bool CanAct() const { return (bIsAttacking || bIsHealing || bIsRolling || bIsOnHit) == false; };

	FORCEINLINE bool IsFreeOfAction() const
	{
		return CanAct() && bIsOnHit == false && bIsRolling == false && bIsComboActive == false;
	}

	FORCEINLINE bool IsMovingForward() const
	{
		FVector MovingDirection = GetVelocity();
		MovingDirection.Z = 0.0f;
		MovingDirection.Normalize();
		return FVector::DotProduct(MovingDirection, GetActorForwardVector())
			> FMath::Cos(HalfRunningAngle / 180.0f * PI);
	}

	FORCEINLINE bool IsGettingForwardInput() const { return VerticalInput > 0; }

	// Getter
	UFUNCTION(BlueprintCallable)
	virtual int32 GetMaxHealth() const override { return CurLevelData->MaxHealth; }

	virtual int32 GetCalculatedDamage() const override;

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
	
	UFUNCTION(BlueprintCallable)
	virtual void ReceiveDamage(int32 Damage) override;

	UFUNCTION(BlueprintCallable)
	void ReceiveHeal();

	UFUNCTION(BlueprintCallable)
	void ReceiveExp(int32 Exp);

	void ExamineRunning();
	void LerpSpeed(const float DeltaTime);
	void UpdateEnergy(const float DeltaTime);

	FORCEINLINE void ResetCombo()
	{
		CurCombo = 0;
		bIsComboActive = false;
	}

	FORCEINLINE void GoToNextCombo() { CurCombo = (CurCombo + 1) % MaxCombo; }
};
