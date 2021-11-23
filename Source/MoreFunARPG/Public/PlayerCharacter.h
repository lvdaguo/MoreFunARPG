#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

DECLARE_EVENT_TwoParams(AGameCharacter, FHealthChange, int, int)

UCLASS()
class MOREFUNARPG_API APlayerCharacter final : public ACharacter
{
	GENERATED_BODY()

	// Constructor
public:
	APlayerCharacter();

	// Life Cycle
private:
	virtual void Tick(const float DeltaTime) override;
	void LerpSpeed(float DeltaTime);
	virtual void BeginPlay() override;

	// Action
	UFUNCTION(BlueprintCallable)
	int32 OnBeginPrimaryAttack();

	UFUNCTION(BlueprintCallable)
	void OnEndPrimaryAttack();

	UFUNCTION(BlueprintCallable)
	bool OnBeginHealing();

	UFUNCTION(BlueprintCallable)
	void OnEndHealing();

	UFUNCTION(BlueprintCallable)
	bool OnBeginRolling();

	UFUNCTION(BlueprintCallable)
	void OnEndRolling();

	UFUNCTION(BlueprintCallable)
	void OnBeginRunning();

	UFUNCTION(BlueprintCallable)
	void OnEndRunning();

	// Axis
	UFUNCTION(BlueprintCallable)
	void OnMoveRight(const float Value);

	UFUNCTION(BlueprintCallable)
	void OnMoveForward(const float Value);

	UFUNCTION(BlueprintCallable)
	void OnTurn(const float Value);

	UFUNCTION(BlueprintCallable)
	void OnLookUp(const float Value);

	// Attribute
	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	int32 MaxHealth;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float RunningSpeed;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float WalkSpeed;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float SwitchRunningTime;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	int32 MaxEnergy;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	int32 RunEnergyCost;


	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	TArray<int32> ComboDamageList;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float ComboResetDelayTime;
	
	// Runtime Member
	int32 CurEnergy;
	int32 CurHealth;

	int32 MaxCombo;
	int32 CurCombo;

	bool bNeedLerp;
	float TargetMovingSpeed;
	float LerpTime;

	UPROPERTY()
	class UCharacterMovementComponent* Movement;

	// struct	
	FTimerHandle ComboResetTimerHandle;

	// Getter
	UPROPERTY(BlueprintGetter = IsAttacking)
	bool bIsAttacking;

	UPROPERTY(BlueprintGetter = IsRolling)
	bool bIsRolling;

	UPROPERTY(BlueprintGetter = IsHealing)
	bool bIsHealing;

	UPROPERTY(BlueprintGetter = IsDead)
	bool bIsDead;
	
public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsAttacking() const { return bIsAttacking; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsRolling() const { return bIsRolling; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsHealing() const { return bIsHealing; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsDead() const { return bIsDead; }
	
private:
	// Component
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* CharacterMesh;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* FollowCamera;
	
	// Init
	void SetupComponent();
	void SetupAttachment() const;
	void SetupComponentDefaultValues() const;
	void SetupAttributeDefaultValues();

	void SetupRuntimeValues();

public:
	// Event / Delegate
	FORCEINLINE FHealthChange& HealthChangeEvent() { return HealthChange; }
	
private:
	FHealthChange HealthChange;

	void ChangeHealth(int Diff);
	FORCEINLINE void ResetCombo()
	{
		UE_LOG(LogTemp, Log, TEXT("Combo reset!"))
		CurCombo = 0;
	}
	FORCEINLINE void NextCombo() { CurCombo = (CurCombo + 1) % MaxCombo; }
};
