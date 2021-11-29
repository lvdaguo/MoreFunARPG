#pragma once

#include "CoreMinimal.h"
#include "ARPGCharacter.h"
#include "MonsterDataTableRow.h"
#include "MonsterCharacter.generated.h"

UCLASS()
class MOREFUNARPG_API AMonsterCharacter final : public AARPGCharacter
{
	GENERATED_BODY()

	// Constructor
public:
	AMonsterCharacter();

	// Setup Default
	virtual void SetupDataFromDataTable() override;
	
	// Life Cycle
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Attribute
	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	TArray<class USkeletalMesh*> RandomMeshPool; 
	
	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float PatrolSpeed = 400.0f;

	UPROPERTY(EditDefaultsOnly, Category="Attribute")
	float ChaseSpeed = 800.0f;

	// Data From DataTable
	TArray<struct FMonsterDataTableRow*> AllLevelData;
	FMonsterDataTableRow* CurLevelData;

public:
	// Getter
	FORCEINLINE virtual int32 GetMaxHealth() const override { return CurLevelData->MaxHealth; }
	FORCEINLINE virtual int32 GetDamage() const override { return CurLevelData->Damage; }
	FORCEINLINE int32 GetExpWorth() const { return CurLevelData->ExpWorth; }
	
protected:
	// Action
	UFUNCTION(BlueprintCallable)
	bool BeginAttack();

	UFUNCTION(BlueprintCallable)
	void EndAttack();

	UFUNCTION(BlueprintCallable)
	bool BeginHealing();

	UFUNCTION(BlueprintCallable)
	void EndHealing();

	UFUNCTION(BlueprintCallable)
	bool BeginRunning();

	UFUNCTION(BlueprintCallable)
	void EndRunning();
};
