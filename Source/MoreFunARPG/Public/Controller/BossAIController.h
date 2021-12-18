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

	class FDelegateHandle PlayerDieHandle;
	class FDelegateHandle PlayerRespawnHandle;
	
	// Life Cycle
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:
	// Task Helper
	UFUNCTION(BlueprintCallable)
	void StartChargeCoolDownCountDown();
	void RemoveListener() const;
	
protected:
	// Override
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
};
