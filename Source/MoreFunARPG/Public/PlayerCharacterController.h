#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"

/**
 * 
 */
UCLASS()
class MOREFUNARPG_API APlayerCharacterController final : public APlayerController
{
	GENERATED_BODY()

public:
	APlayerCharacterController();

	virtual void BeginPlay() override; 
	
private:
	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;
	
	// virtual void SetupInputComponent() override;
	//
	// // Action
	// void OnPrimaryAttack();
	// void OnHeal();
	// void OnRoll();
	// void OnJump();
	// void OnStartRunning();
	// void OnStopRunning();
	//
	// // Axis
	// void OnMoveRight(const float Value);
	// void OnMoveForward(const float Value);
	// void OnTurn(const float Value);
	// void OnLookUp(const float Value);
};
