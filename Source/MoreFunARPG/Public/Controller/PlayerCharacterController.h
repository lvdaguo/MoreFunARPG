#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"

UCLASS()
class MOREFUNARPG_API APlayerCharacterController final : public APlayerController
{
	GENERATED_BODY()

protected:
	// Runtime
	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;

	void SetupDelegate();
	void SetupInputAxis();
	void SetupInputAction();
	// Life Cycle
	virtual void BeginPlay() override; 

	// Binding
	void OnPrimaryAttackPressed();
	void OnHealPressed();
	void OnRunningPressed();
	void OnRunningReleased();
	void OnRollPressed();
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	// Override
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void SetupInputComponent() override;
	void ClearInputComponent() const;

	// Listener
	UFUNCTION()
	void OnPlayerRespawned();
};
