#pragma once

#include "CoreMinimal.h"
#include "ARPGCharacter.h"
#include "DataTableRow/PlayerDataTableRow.h"
#include "Camera/CameraComponent.h"
#include "PlayerCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FPlayerDie);
DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerCameraLocationUpdate, FVector);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameOver);

UCLASS()
class MOREFUNARPG_API APlayerCharacter final : public AARPGCharacter
{
	GENERATED_BODY()

	// Constructor
public:
	APlayerCharacter();

	// Life Cycle
protected:
	virtual void BeginPlay() override;
	virtual void Tick(const float DeltaTime) override;

	// Setup Default
	void SetupComponent();
	void SetupAttachment() const;
	void SetupComponentDefaultValues() const;

	// Setup Runtime
	virtual void SetupState() override;
	void SetupDataFromDataTable();
	void SetupCombo();
	void SetupRuntimeValues();
	virtual void SetupDelegate() override;

	// Component
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* CharacterMesh;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* FollowCamera;

	// Attribute
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attribute")
	float MaxEnergy = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float RunningEnergyCost = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float RunningEnergyRefuel = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	TArray<float> ComboDamageRateList = {1.0f, 1.5f, 1.25f, 3.0f};

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
	int32 DefaultHealPotion = 1;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	int32 HealAmount = 50;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attribute")
	int32 PlayerLife = 3;

	UPROPERTY(BlueprintReadOnly)
	int32 PlayerScore = 0;

	// Runtime Member
	UPROPERTY(BlueprintReadOnly)
	float CurEnergy;

	int32 MaxCombo;
	int32 CurCombo;
	FTimerHandle ComboResetTimerHandle;

	int32 CalculatedDamage;

	UPROPERTY(BlueprintReadOnly)
	int32 CurHealPotion;

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
	bool bIsOnHit;
	bool bIsRunning;

	UPROPERTY(BlueprintReadOnly)
	bool bIsMaxRunningSpeed;

	void ResetStates();

	// Condition
	FORCEINLINE bool CanAct() const { return (bIsAttacking || bIsHealing || bIsRolling || bIsOnHit) == false; };

	FORCEINLINE bool IsFreeOfAction() const
	{
		return CanAct() && bIsOnHit == false && bIsRolling == false && bIsComboActive == false;
	}

	FORCEINLINE bool IsMovingForward() const
	{
		const FVector MovingDirection = GetVelocity().GetSafeNormal();
		return FVector::DotProduct(MovingDirection, GetActorForwardVector())
			> FMath::Cos(HalfRunningAngle / 180.0f * PI);
	}

	FORCEINLINE bool IsGettingForwardInput() const { return VerticalInput > 0; }

	// Getter
	UFUNCTION(BlueprintCallable)
	virtual int32 GetMaxHealth() const override { return CurLevelData->MaxHealth; }

	UFUNCTION(BlueprintCallable)
	int32 GetArmor() const { return CurLevelData->Armor; }

	UFUNCTION(BlueprintCallable)
	int32 GetExpNeededToNextLevel() const { return CurLevelData->ExpNeededToNextLevel; }

public:
	FORCEINLINE FVector GetCameraWorldLocation() const { return FollowCamera->GetComponentLocation(); }

protected:
	int32 GetNormalDamage() const { return CurLevelData->NormalDamage; }
	int32 GetCriticalDamage() const { return CurLevelData->CriticalDamage; }
	float GetCriticalHitRate() const { return CurLevelData->CriticalHitRate; }

	// Override
	virtual int32 GetCalculatedDamage() const override;
	void CalculateDamage();
	
	UFUNCTION(BlueprintCallable)
	virtual void OnWeaponOverlap(AActor* OtherActor) override;

	virtual void Die() override;

	UFUNCTION(BlueprintCallable)
	virtual void ReceiveDamage(int32 Damage) override;

	// Heal
	UFUNCTION(BlueprintCallable)
	void ReceiveHeal();

	// Weapon
	UFUNCTION(BlueprintCallable)
	void EnableWeapon(class UPrimitiveComponent* WeaponHitBox);

	UFUNCTION(BlueprintCallable)
	void DisableWeapon(class UPrimitiveComponent* WeaponHitBox);

	// Level
	void LevelUp();
	void ReceiveExp(int32 Exp);

	// Running
	void ExamineRunning();
	void LerpSpeed(const float DeltaTime);
	void UpdateEnergy(const float DeltaTime);

	// Combo
	FORCEINLINE void ResetCombo()
	{
		CurCombo = 0;
		bIsComboActive = false;
	}

	FORCEINLINE void GoToNextCombo()
	{
		CurCombo = (CurCombo + 1) % MaxCombo;
	}

	// Listener
	UFUNCTION()
	void OnExpUpdated(const int32 Exp);

	UFUNCTION(BlueprintCallable)
	void OnHealthPotionOverlap(class AHealPotion* Potion);

	UFUNCTION()
	void OnPlayerRespawn();

	UFUNCTION()
	void OnPlayerScoreUpdated(int32 Score);

	// Delegate
	FPlayerDie PlayerDie;
	FPlayerCameraLocationUpdate PlayerCameraLocationUpdate;

	UPROPERTY(BlueprintAssignable)
	FGameOver GameOver;
	
public:
	FORCEINLINE FPlayerDie& PlayerDieEvent() { return PlayerDie; }
	FORCEINLINE FPlayerCameraLocationUpdate& PlayerCameraLocationUpdateEvent()
	{
		return PlayerCameraLocationUpdate;
	}

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

	// Blueprint Implementable Event
	UFUNCTION(BlueprintImplementableEvent)
	void OnInvincibleBegin();

	UFUNCTION(BlueprintImplementableEvent)
	void OnInvincibleEnd();

	UFUNCTION(BlueprintImplementableEvent)
	void OnHit();

	// Action
	// (implemented in PlayerCharacter blueprint
	// bind in PlayerController cpp script) 
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
};
