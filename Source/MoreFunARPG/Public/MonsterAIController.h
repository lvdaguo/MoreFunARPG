#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "MonsterAIController.generated.h"

/**
 * 
 */
UCLASS()
class MOREFUNARPG_API AMonsterAIController final : public AAIController
{
	GENERATED_BODY()

public:
	AMonsterAIController();

protected:
	UPROPERTY(VisibleAnywhere)
	class UAIPerceptionComponent* AIPerception;

	UPROPERTY(EditDefaultsOnly)
	class UBehaviorTree* BehaviourTree;

	UPROPERTY()
	class AMonsterCharacter* MonsterCharacter;
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnMonsterHealthChange(int32 Before, int32 After);

	UFUNCTION(BlueprintCallable)
	void OnTargetPerceptionUpdated(class AActor* Actor, const struct FAIStimulus& InStimulus);

	virtual void OnPossess(APawn* InPawn) override;
};
