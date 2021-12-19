#include "ARPGCharacter/PlayerCharacter.h"

#include "HealPotion.h"
#include "Spawner.h"
#include "Camera/CameraComponent.h"
#include "Chaos/KinematicTargets.h"
#include "Components/ShapeComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Constructor
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SetupComponent();
	SetupAttachment();
	SetupComponentDefaultValues();

	// just set once
	// don't reset when respawn
	PlayerScore = 0;
}

// Setup Default
void APlayerCharacter::SetupComponent()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
}

void APlayerCharacter::SetupAttachment() const
{
	SpringArm->SetupAttachment(GetMesh());
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
}

void APlayerCharacter::SetupComponentDefaultValues() const
{
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FQuat(FRotator(0.0f, -90.0f, 0.0f)));

	SpringArm->bUsePawnControlRotation = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bIgnoreBaseRotation = true;
}

// Setup Runtime
void APlayerCharacter::SetupDataFromDataTable()
{
	check(LevelDataTable != nullptr)

	LevelDataTable->GetAllRows(nullptr, AllLevelData);
	CurLevelData = AllLevelData[0];

	CurExpGained = 0;
	MaxLevel = AllLevelData.Num();
	CurLevel = 1;
	MaxHealth = CurLevelData->MaxHealth;
}

void APlayerCharacter::SetupCombo()
{
	CurCombo = 0;
	MaxCombo = ComboDamageRates.Num();
}

void APlayerCharacter::SetupState()
{
	bIsAttacking = false;
	bIsComboActive = false;
	bIsHealing = false;
	bIsRolling = false;
	bIsRunning = false;
	bIsOnHit = false;
	bIsMaxRunningSpeed = false;
	bIsInvincible = false;
}

void APlayerCharacter::SetupRuntimeValues()
{
	CurHealth = MaxHealth;
	TargetMovingSpeed = WalkSpeed;
	VerticalInput = 0.0f;
	CurEnergy = MaxEnergy;
	CurHealPotion = DefaultHealPotion;
	CalculatedDamage = 0;
}

void APlayerCharacter::SetupDelegate()
{
	Super::SetupDelegate();
	ASpawner* Spawner = Cast<ASpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawner::StaticClass()));

	Spawner->PlayerExpUpdateEvent().AddLambda([this](const int32 Exp)
	{
		ReceiveExp(Exp);
	});
	Spawner->PlayerScoreUpdateEvent().AddLambda([this](const int32 Score)
	{
		PlayerScore += Score;
	});

	Spawner->PlayerRespawnEvent().AddUObject(this, &APlayerCharacter::OnPlayerRespawn);
}

// Life Cycle
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetupDataFromDataTable();
	SetupCombo();
	SetupRuntimeValues();

	const AActor* Start = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());
	SpawnPosition = Start->GetActorLocation();
}

void APlayerCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	PlayerCameraLocationUpdateEvent().Broadcast(GetCameraWorldLocation());

	if (bIsDead)
	{
		return;
	}

	ExamineRunning();
	LerpSpeed(DeltaTime);
	UpdateEnergy(DeltaTime);
}

// Level
void APlayerCharacter::LevelUp()
{
	if (CurLevel == MaxLevel)
	{
		return;
	}
	const int32 CurLevelIndex = CurLevel - 1;
	CurLevelData = AllLevelData[CurLevelIndex + 1];
	MaxHealth = CurLevelData->MaxHealth;
	CurHealth = MaxHealth;
	CurLevel++;
}

void APlayerCharacter::ReceiveExp(const int32 Exp)
{
	CurExpGained += Exp;
	while (CurExpGained >= GetExpNeededToNextLevel())
	{
		if (CurLevel == MaxLevel)
		{
			CurExpGained = GetExpNeededToNextLevel();
			break;
		}
		CurExpGained -= GetExpNeededToNextLevel();
		LevelUp();
	}
}

// States
void APlayerCharacter::ResetStates()
{
	EndPrimaryAttack();
	EndHealing();
	EndInvincible();
	EndRolling();
	EndOnHit();
	EndRunning();
	bIsDead = false;
}

// Override
void APlayerCharacter::OnWeaponOverlap(AActor* OtherActor)
{
	Super::OnWeaponOverlap(OtherActor);
}

void APlayerCharacter::Die()
{
	Super::Die();
	if (PlayerLife <= 0)
	{
		// game over
		GameOver.Broadcast();
	}
	else
	{
		PlayerLife--;
	}
	PlayerDie.Broadcast();
}

void APlayerCharacter::CalculateDamage()
{
	const bool IsCriticalHit = FMath::FRand() <= GetCriticalHitRate();
	int32 Damage = IsCriticalHit ? GetCriticalDamage() : GetNormalDamage();
	Damage *= ComboDamageRates[CurCombo];
	CalculatedDamage = Damage;
}

int32 APlayerCharacter::GetCalculatedDamage() const
{
	// damage was calculated in every attack 
	return CalculatedDamage;
}

void APlayerCharacter::ReceiveDamage(const int32 Damage)
{
	if (Damage <= 0 || bIsInvincible)
	{
		return;
	}

	const int32 FinalDamage = Damage - GetArmor();
	if (FinalDamage > 0)
	{
		ChangeHealthBase(-1 * FinalDamage);
		if (bIsDead == false)
		{
			OnHit();
		}
	}
}

// Running 
void APlayerCharacter::ExamineRunning()
{
	if (bIsRunning && IsInNullActionState() && IsFacingForward() == false)
	{
		EndRunning();
	}
}

void APlayerCharacter::LerpSpeed(const float DeltaTime)
{
	if (bIsRunning && CanAct() == false)
	{
		return;
	}
	bIsMaxRunningSpeed = false;
	if (FMath::IsNearlyEqual(GetCharacterMovement()->MaxWalkSpeed, TargetMovingSpeed))
	{
		if (TargetMovingSpeed == RunningSpeed)
		{
			bIsMaxRunningSpeed = true;
		}
		return;
	}
	const float Alpha = LerpTime / SwitchRunningTime;
	GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(GetCharacterMovement()->MaxWalkSpeed, TargetMovingSpeed, Alpha);
	LerpTime += DeltaTime;
}

void APlayerCharacter::UpdateEnergy(const float DeltaTime)
{
	if (bIsRunning && IsFacingForward() && IsInNullActionState())
	{
		// attack can be interrupted now but is still playing the end of the attack animation
		if (bIsComboActive && IsPlayingNetworkedRootMotionMontage())
		{
			StopAnimMontage();
		}
		CurEnergy -= RunningEnergyCost * DeltaTime;
		if (CurEnergy <= 0.0f)
		{
			EndRunning();
		}
	}
	else if (bIsRolling == false)
	{
		if (CurEnergy >= MaxEnergy)
		{
			return;
		}
		CurEnergy += RunningEnergyRefuel * DeltaTime;
	}
}

// Listener
void APlayerCharacter::OnPlayerRespawn()
{
	ResetStates();
	SetActorEnableCollision(true);
	SetupRuntimeValues();
	SetActorLocation(SpawnPosition);
	GetCharacterMovement()->ClearAccumulatedForces();
}

// Action Blueprint Implementation Helper
bool APlayerCharacter::BeginRunning()
{
	if (IsGettingForwardInput() == false)
	{
		return FAIL;
	}

	bIsRunning = true;
	TargetMovingSpeed = RunningSpeed;
	LerpTime = 0.0f;

	return SUCCESS;
}

void APlayerCharacter::EndRunning()
{
	if (bIsRunning == false)
	{
		return;
	}

	bIsRunning = false;
	TargetMovingSpeed = WalkSpeed;
	LerpTime = 0.0f;
}

bool APlayerCharacter::BeginPrimaryAttack(int32& OutComboAnimIndex)
{
	if (CanAct() == false)
	{
		// ignore new input if player is already doing action
		return FAIL;
	}

	bIsAttacking = true;

	CalculateDamage();

	// cancel combo reset
	GetWorldTimerManager().ClearTimer(ComboResetTimerHandle);
	OutComboAnimIndex = CurCombo;

	return SUCCESS;
}

void APlayerCharacter::EndPrimaryAttack()
{
	if (bIsAttacking == false)
	{
		return;
	}

	bIsAttacking = false;
	bIsComboActive = true;
	GoToNextCombo();

	// reset combo after some time
	GetWorldTimerManager().SetTimer(ComboResetTimerHandle, this,
	                                &APlayerCharacter::ResetCombo, ComboResetDelayTime, false);
}

bool APlayerCharacter::BeginHealing()
{
	const bool FailCondition = CanAct() == false || CurHealPotion <= 0;
	return BeginActionBase(bIsHealing, FailCondition);
}

void APlayerCharacter::EndHealing()
{
	EndActionBase(bIsHealing);
}

bool APlayerCharacter::BeginRolling()
{
	if (bIsRolling || bIsOnHit || CurEnergy < RollEnergyCost)
	{
		return FAIL;
	}
	// interrupt from other states
	EndHealing();
	EndPrimaryAttack();
	GetWorldTimerManager().ClearTimer(ComboResetTimerHandle);
	ResetCombo();

	CurEnergy -= RollEnergyCost;
	bIsRolling = true;

	return SUCCESS;
}

void APlayerCharacter::EndRolling()
{
	EndActionBase(bIsRolling);
}

bool APlayerCharacter::BeginOnHit()
{
	if (bIsOnHit || bIsAttacking)
	{
		// if doing attacking action
		// the damage still exists but the on hit wont be played
		// means that attacking action wont be interrupted
		return FAIL;
	}
	EndHealing();
	EndRolling();

	bIsOnHit = true;
	return SUCCESS;
}

void APlayerCharacter::EndOnHit()
{
	EndActionBase(bIsOnHit);
}

bool APlayerCharacter::BeginInvincible()
{
	if (bIsInvincible)
	{
		return FAIL;
	}
	bIsInvincible = true;
	OnInvincibleBegin();

	return SUCCESS;
}

void APlayerCharacter::EndInvincible()
{
	if (bIsInvincible == false)
	{
		return;
	}
	bIsInvincible = false;
	OnInvincibleEnd();
}

// Heal
void APlayerCharacter::ReceiveHeal()
{
	CurHealPotion--;
	ChangeHealthBase(HealAmount);
}

// Weapon
void APlayerCharacter::EnableWeapon(UPrimitiveComponent* WeaponHitBox)
{
	Super::EnableWeapon(WeaponHitBox);
}

void APlayerCharacter::DisableWeapon(UPrimitiveComponent* WeaponHitBox)
{
	Super::DisableWeapon(WeaponHitBox);
}

void APlayerCharacter::OnHealthPotionOverlap(AHealPotion* Potion)
{
	// if cast from blueprint not fail
	if (Potion != nullptr)
	{
		CurHealPotion++;
		Potion->Destroy();
	}
}

// Axis
void APlayerCharacter::MoveForward(const float Value)
{
	VerticalInput = Value;
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(const float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::Turn(const float Value)
{
	AddControllerYawInput(Value);
}

void APlayerCharacter::LookUp(const float Value)
{
	AddControllerPitchInput(Value);
}
