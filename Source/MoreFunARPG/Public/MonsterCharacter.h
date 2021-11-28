#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonsterCharacter.generated.h"

UCLASS()
class MOREFUNARPG_API AMonsterCharacter final : public ACharacter
{
	GENERATED_BODY()

	// Constructor
public:
	AMonsterCharacter();

	// Life Cycle
private:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Attribute
	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float MoveSpeed;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	int32 AttackDamage;
	
	// Action
	UFUNCTION(BlueprintCallable)
	void BeginAttack();

	UFUNCTION(BlueprintCallable)
	void EndAttack();

	UFUNCTION(BlueprintCallable)
	void BeginHealing();

	UFUNCTION(BlueprintCallable)
	void EndHealing();

	UFUNCTION(BlueprintCallable)
	void BeginRunning();

	UFUNCTION(BlueprintCallable)
	void EndRunning();
};
