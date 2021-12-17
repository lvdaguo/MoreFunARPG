#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BossAIController.generated.h"

UCLASS()
class MOREFUNARPG_API ABossAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Constructor
	ABossAIController();

protected:
	// Setup Runtime
	void SetupDelegate();

	// Property
	UPROPERTY(EditDefaultsOnly)
	class UBehaviorTree* BehaviourTree;

	UPROPERTY()
	class ABossCharacter* BossCharacter;

	// Life Cycle
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// Listener
	UFUNCTION()
	void OnPlayerDead();

	UFUNCTION()
	void OnPlayerRespawn();

public:
	UFUNCTION(BlueprintCallable)
	void StartChargeCoolDownCountDown();

protected:
	void SetChargeReady();

	// Override
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
};
