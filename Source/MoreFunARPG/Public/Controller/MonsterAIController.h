#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
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

	// Life Cycle
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// Listener
	UFUNCTION()
	void OnPlayerDead();

	UFUNCTION()
	void OnPlayerRespawn();
	
	UFUNCTION()
	void OnMonsterHealthChange(int32 Before, int32 After);

	UFUNCTION(BlueprintCallable)
	void OnTargetPerceptionUpdated(class AActor* Actor, const struct FAIStimulus& InStimulus);

	// Override
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
};
