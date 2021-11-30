// #pragma once
//
// #include "CoreMinimal.h"
// #include "ARPGCharacter.h"
// #include "HumanLikeCharacter.generated.h"
//
// UCLASS()
// class MOREFUNARPG_API AHumanLikeCharacter : public AARPGCharacter
// {
// 	GENERATED_BODY()
//
// 	// Constructor
// 	public:
// 	AHumanLikeCharacter();
//
// 	// Life Cycle
// 	protected:
// 	virtual void Tick(const float DeltaTime) override;
// 	virtual void BeginPlay() override;
//
// 	// Setup Default
// 	virtual void SetupStateDefaultValues() override;
// 	void LerpSpeed(float DeltaTime);
//
// 	// Attribute
// 	UPROPERTY(EditDefaultsOnly, Category="Attribute")
// 	float WalkSpeed = 600.0f;
//
// 	UPROPERTY(EditDefaultsOnly, Category="Attribute")
// 	float RunningSpeed = 1000.0f;
//
// 	UPROPERTY(EditDefaultsOnly, Category="Attribute")
// 	float SwitchRunningTime = 2.0f;
//
// 	UPROPERTY(EditDefaultsOnly, Category="Attribute")
// 	float DefaultInvisibleTime = 2.0f;
//
// 	// States
// 	
// 	bool bIsRunning;
// 	float LerpTime;
// 	float TargetMovingSpeed;
//
// 	// Runtime Member
// 	UPROPERTY(BlueprintReadOnly)
// 	float CurEnergy;
//
// 	UFUNCTION(BlueprintCallable)
// 	bool BeginRunning();
//
// 	UFUNCTION(BlueprintCallable)
// 	void EndRunning();
//
// 	UFUNCTION(BlueprintCallable)
// 	bool BeginOnHit();
//
// 	UFUNCTION(BlueprintCallable)
// 	void EndOnHit();
//
// 	bool BeginInvincible();
//
// 	void EndInvincible();
// };
