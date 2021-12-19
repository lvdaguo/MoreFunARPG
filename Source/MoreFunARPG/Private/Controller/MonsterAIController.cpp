#include "Controller/MonsterAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Spawner.h"
#include "ARPGCharacter/MonsterCharacter.h"
#include "ARPGCharacter/PlayerCharacter.h"
#include "GlobalNameText.h"
#include "MacroAIHelper.h"

// Constructor
AMonsterAIController::AMonsterAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

// Setup Runtime
void AMonsterAIController::SetupDelegate()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(
		UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	PlayerDieHandle = PlayerCharacter->PlayerDieEvent().AddLambda([this]()
	{
		BB_SET_BOOL(MonsterAIController::IsPlayerDead, true);
	});
	
	ASpawner* Spawner = Cast<ASpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawner::StaticClass()));
	PlayerRespawnHandle = Spawner->PlayerRespawnEvent().AddLambda([this]()
	{
		BB_SET_BOOL(MonsterAIController::IsPlayerDead, false);
	});
	
	BB_SET_OBJECT(MonsterAIController::PlayerActor, PlayerCharacter)
}

// Life Cycle
void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BehaviourTree);

	BB_SET_BOOL(MonsterAIController::IsPlayerDead, false)
	SetupDelegate();
}

void AMonsterAIController::Tick(float DeltaSeconds)
{
	using namespace MonsterAIController;
	Super::Tick(DeltaSeconds);

	const bool bIsPlayerInSight = BB_GET_BOOL(IsPlayerInSight);
	if (bIsPlayerInSight)
	{
		const AActor* PlayerCharacter = Cast<AActor>(BB_GET_OBJECT(PlayerActor));
		BB_SET_VECTOR(MovingPosition, PlayerCharacter->GetActorLocation())
	}
	
	const float Cur = static_cast<float>(MonsterCharacter->GetCurHealth());
	const float Max = static_cast<float>(MonsterCharacter->GetMaxHealth());
	const float HealthPercent = Cur / Max;

	BB_SET_BOOL(IsLowHealth, HealthPercent <= MonsterCharacter->GetLowHealthPercent())
	BB_SET_BOOL(HasHealthPotion, MonsterCharacter->GetHealthPotion() > 0)
}

// Listener
void AMonsterAIController::OnTargetPerceptionUpdated(AActor* Actor, const FAIStimulus& InStimulus)
{
	if (Actor->IsA(APlayerCharacter::StaticClass()))
	{
		BB_SET_BOOL(MonsterAIController::IsPlayerInSight, InStimulus.WasSuccessfullySensed())
	}
}

void AMonsterAIController::RemoveListener() const
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(
		UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	ASpawner* Spawner = Cast<ASpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawner::StaticClass()));
	PlayerCharacter->PlayerDieEvent().Remove(PlayerDieHandle);
	if (Spawner != nullptr)
	{
		Spawner->PlayerRespawnEvent().Remove(PlayerRespawnHandle);
	}
}

// Override
void AMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	MonsterCharacter = Cast<AMonsterCharacter>(InPawn);
	MonsterCharacter->HealthChangeEvent().AddLambda([this](const int32 Before, const int32 After)
	{
		if (Before > After)
		{
			BB_SET_BOOL(MonsterAIController::IsPlayerInSight, true)
		}
	});
}

void AMonsterAIController::OnUnPossess()
{
	Super::OnUnPossess();
	RemoveListener();
	Destroy();
}
