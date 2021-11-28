#pragma once

#include "CoreMinimal.h"
#include "ARPGCharacter.h"
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

	// Action
	UFUNCTION(BlueprintCallable)
	int32 BeginPrimaryAttack();

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
	void BeginRunning();

	UFUNCTION(BlueprintCallable)
	void EndRunning();

	UFUNCTION(BlueprintCallable)
	bool BeginOnHit();

	UFUNCTION(BlueprintCallable)
	void EndOnHit();

	// Axis
	UFUNCTION(BlueprintCallable)
	void MoveRight(const float Value);

	UFUNCTION(BlueprintCallable)
	void MoveForward(const float Value);

	UFUNCTION(BlueprintCallable)
	void Turn(const float Value);

	UFUNCTION(BlueprintCallable)
	void LookUp(const float Value);

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

	// Runtime Member
	UPROPERTY(BlueprintReadOnly)
	float CurEnergy;

	int32 MaxCombo;
	int32 CurCombo;
	FTimerHandle ComboResetTimerHandle;

	float TargetMovingSpeed;
	float LerpTime;

	// States
	bool bIsAttacking;
	bool bIsRolling;
	bool bIsHealing;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDead;

	bool bIsOnHit;
	bool bIsRunning;

	FORCEINLINE bool CanMove() const { return (bIsAttacking || bIsHealing || bIsRolling || bIsOnHit) == false; };

	// Component
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* CharacterMesh;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* FollowCamera;

	// Operation 
	void LerpSpeed(const float DeltaTime);
	void UpdateEnergy(const float DeltaTime);

	FORCEINLINE void ResetCombo()
	{
		UE_LOG(LogTemp, Log, TEXT("Combo reset!"))
		CurCombo = 0;
	}

	FORCEINLINE void NextCombo() { CurCombo = (CurCombo + 1) % MaxCombo; }


	float HorizontalInput;
	float VerticalInput;

	FORCEINLINE bool IsGettingMovementInput() const
	{
		return FMath::IsNearlyZero(HorizontalInput) == false
			|| FMath::IsNearlyZero(VerticalInput) == false;
	}
};
