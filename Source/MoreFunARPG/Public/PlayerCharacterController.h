#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"

UCLASS()
class MOREFUNARPG_API APlayerCharacterController final : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;

	virtual void BeginPlay() override; 

	virtual void SetupInputComponent() override;
	void OnPrimaryAttackPressed();
	void OnHealPressed();
	void OnRunningPressed();
	void OnRunningReleased();
	void OnRollPressed();
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	virtual void OnUnPossess() override;

	UFUNCTION()
	void OnPlayerRespawned(APlayerCharacter* RespawnedPlayer);
};
