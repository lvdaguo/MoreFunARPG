#include "Controller/MonsterAIController.h"

#include "Spawner.h"
#include "ARPGCharacter/MonsterCharacter.h"
#include "ARPGCharacter/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"

AMonsterAIController::AMonsterAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

void AMonsterAIController::SetupDelegate()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(
		UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	PlayerCharacter->PlayerDieEvent().AddUObject(this, &AMonsterAIController::OnPlayerDead);

	ASpawner* Spawner = Cast<ASpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawner::StaticClass()));
	Spawner->PlayerRespawnEvent().AddUObject(this, &AMonsterAIController::OnPlayerRespawn);
}

void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BehaviourTree);

	OnPlayerRespawn();
	SetupDelegate();
	
}

void AMonsterAIController::OnPlayerDead()
{
	static const FName IsPlayerDead(TEXT("IsPlayerDead"));
	GetBlackboardComponent()->SetValueAsBool(IsPlayerDead, true);
}

void AMonsterAIController::OnPlayerRespawn()
{
	static const FName IsPlayerDead(TEXT("IsPlayerDead"));
	GetBlackboardComponent()->SetValueAsBool(IsPlayerDead, false);
}

void AMonsterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	static const FName IsLowHealth(TEXT("IsLowHealth"));
	static const FName HasHealthPotion(TEXT("HasHealPotion"));
	static const FName IsPlayerInSight(TEXT("IsPlayerInSight"));

	const bool bIsPlayerInSight = GetBlackboardComponent()->GetValueAsBool(IsPlayerInSight);
	if (bIsPlayerInSight)
	{
		static const FName MovingPosition(TEXT("MovingPosition"));
		static const FName PlayerActor(TEXT("PlayerActor"));

		const AActor* PlayerCharacter = Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(PlayerActor));
		if (PlayerCharacter != nullptr)
		{
			GetBlackboardComponent()->SetValueAsVector(MovingPosition, PlayerCharacter->GetActorLocation());
		}
	}
	
	const float Cur = static_cast<float>(MonsterCharacter->GetCurHealth());
	const float Max = static_cast<float>(MonsterCharacter->GetMaxHealth());
	const float HealthPercent = Cur / Max;

	GetBlackboardComponent()->SetValueAsBool(IsLowHealth,
	                                         HealthPercent <= MonsterCharacter->GetLowHealthPercent());
	GetBlackboardComponent()->SetValueAsBool(HasHealthPotion,
	                                         MonsterCharacter->GetHealthPotion() > 0);
}

void AMonsterAIController::OnMonsterHealthChange(const int32 Before, const int32 After)
{
	if (Before > After)
	{
		static const FName IsPlayerInSight(TEXT("IsPlayerInSight"));
		GetBlackboardComponent()->SetValueAsBool(IsPlayerInSight, true);
	}
}

void AMonsterAIController::OnTargetPerceptionUpdated(AActor* Actor, const FAIStimulus& InStimulus)
{
	if (Actor->IsA(APlayerCharacter::StaticClass()))
	{
		static const FName IsPlayerInSight(TEXT("IsPlayerInSight"));
		GetBlackboardComponent()->SetValueAsBool(IsPlayerInSight,
		                                         InStimulus.WasSuccessfullySensed());

		// update in case player respawns
		static const FName PlayerActor(TEXT("PlayerActor"));
		GetBlackboardComponent()->SetValueAsObject(PlayerActor,
		                                           GetWorld()->GetFirstPlayerController()->GetPawn());
	}
}

void AMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	MonsterCharacter = Cast<AMonsterCharacter>(InPawn);
	check(MonsterCharacter != nullptr)
	MonsterCharacter->HealthChangeEvent().AddUObject(this,
	                                                 &AMonsterAIController::OnMonsterHealthChange);
}

void AMonsterAIController::OnUnPossess()
{
	Super::OnUnPossess();
	Destroy();
}
