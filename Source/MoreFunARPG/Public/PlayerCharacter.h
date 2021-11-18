#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(PlayerCharacterLog, , Log)

DECLARE_EVENT_TwoParams(AGameCharacter, FHealthChange, int, int)

UCLASS()
class MOREFUNARPG_API APlayerCharacter final : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	// Life Cycle
private:
	virtual void Tick(const float DeltaTime) override;
	virtual void BeginPlay() override;

public:
	// Action
	void OnPrimaryAttack();
	void OnHeal();
	void OnRoll();
	void OnJump();
	void OnStartRunning();
	void OnStopRunning();
	
	// Axis
	void OnMoveRight(const float Value);
	void OnMoveForward(const float Value);
	void OnTurn(const float Value);
	void OnLookUp(const float Value);

private:
	// Attribute
	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	int32 MaxHealth;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float RunningSpeed;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float WalkSpeed;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	int32 MaxEnergy;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	int32 RunEnergyCost;

	int32 CurEnergy;
	int32 CurHealth;

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
	void SetupRuntimeValues();
	void SetupAttributeDefaultValues();

public:
	// Event / Delegate
	FORCEINLINE FHealthChange& HealthChangeEvent() { return HealthChange; }
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void ChangeHealth(int Diff);
	FHealthChange HealthChange;
};
