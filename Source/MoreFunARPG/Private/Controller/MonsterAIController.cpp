#include "Controller/MonsterAIController.h"
#include "ARPGCharacter/MonsterCharacter.h"
#include "ARPGCharacter/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"

AMonsterAIController::AMonsterAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BehaviourTree);
}

void AMonsterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	static const FName IsLowHealth(TEXT("IsLowHealth"));
	static const FName HasHealthPotion(TEXT("HasHealPotion"));

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
