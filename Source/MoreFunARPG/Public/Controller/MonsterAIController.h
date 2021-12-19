#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ARPGCharacter/PlayerCharacter.h"
#include "Perception/AIPerceptionTypes.h"
#include "MonsterAIController.generated.h"

UCLASS()
class MOREFUNARPG_API AMonsterAIController final : public AAIController
{
	GENERATED_BODY()

public:
	// Constructor
	AMonsterAIController();

	// Setup Runtime
	void SetupDelegate();

protected:
	// Component
	UPROPERTY(VisibleAnywhere)
	class UAIPerceptionComponent* AIPerception;

	// Property
	UPROPERTY(EditDefaultsOnly)
	class UBehaviorTree* BehaviourTree;

	UPROPERTY()
	class AMonsterCharacter* MonsterCharacter;

	class FDelegateHandle PlayerRespawnHandle;
	class FDelegateHandle PlayerDieHandle;

	// Life Cycle
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// Listener
	UFUNCTION(BlueprintCallable)
	void OnTargetPerceptionUpdated(class AActor* Actor, const struct FAIStimulus& InStimulus);
	void RemoveListener() const;
	
	// Override
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
};
