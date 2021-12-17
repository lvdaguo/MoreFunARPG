#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"

UCLASS()
class MOREFUNARPG_API APlayerCharacterController final : public APlayerController
{
	GENERATED_BODY()

protected:
	// Possessed Pawn
	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;

	// Life Cycle
	virtual void BeginPlay() override; 
	
	// Setup Default
	void SetupDelegate();
	void SetupInputAxis();
	void SetupInputAction();

	// Binding
	void OnPrimaryAttackPressed();
	void OnHealPressed();
	void OnRunningPressed();
	void OnRunningReleased();
	void OnRollPressed();
	void OnMoveForward(float Value);
	void OnMoveRight(float Value);
	void OnTurn(float Value);
	void OnLookUp(float Value);

	// Override
	virtual void OnPossess(class APawn* InPawn) override;
	virtual void OnUnPossess() override;

	// Input Component
	virtual void SetupInputComponent() override;
	void ClearInputComponent() const;
};
