// Fill out your copyright notice in the Description page of Project Settings.


#include "AyoWhoDis.h"

#include "AIController.h"
#include "EnemyBase.h"
#include "EnumLibrary.h"
#include "ProjectWonkyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
AAyoWhoDis::AAyoWhoDis()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	currentHealth = enemyMaxHealth;

	enemyHitbox = CreateDefaultSubobject<UCapsuleComponent>("Hitbox");
	RootComponent = enemyHitbox;

	enemyMeshh = CreateDefaultSubobject<UStaticMeshComponent>("Enemy Mesh_0");
	enemyMeshh->SetupAttachment(GetRootComponent());

	//aggroBox = CreateDefaultSubobject<USphereComponent>("AggroBox");
	//aggroBox->SetupAttachment(GetRootComponent());
	//
	//attackBox = CreateDefaultSubobject<USphereComponent>("AttackBox;");
	//attackBox->SetupAttachment(GetRootComponent());

	currentHealth = enemyMaxHealth;
}

// Called when the game starts or when spawned
void AAyoWhoDis::BeginPlay()
{
	Super::BeginPlay();

	world = GetWorld();

	//attackBox->OnComponentBeginOverlap.AddDynamic(this, &AAyoWhoDis::AAyoWhoDis::AttackRange_BeginOverlap);
	//attackBox->OnComponentEndOverlap.AddDynamic(this, &AAyoWhoDis::AttackRange_EndOverlap);
	//
	//aggroBox->OnComponentBeginOverlap.AddDynamic(this, &AAyoWhoDis::AggroRange_BeginOverlap);
	//aggroBox->OnComponentEndOverlap.AddDynamic(this, &AAyoWhoDis::AggroRange_EndOverlap);

	aiController = Cast<AAIController>(GetController());

	startPos = GetActorLocation();
}

// Called every frame
void AAyoWhoDis::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	switch (currentState)
	{
		// Ich nutze Attack erstmal nur als empty weil ich return �ber idle regeln will und attack eh �ber den timer om overlap
	case EEnemyStates::ES_Attack:
		State_AttackCache();
		break;
		// Idle soll die AI halt auch zu ihrer start pos zur�ck gehen lassen
	case EEnemyStates::ES_Idle:
		State_Idle();
		break;
	case EEnemyStates::ES_MoveToTarget:
		State_MoveToTarget();
		break;

	case EEnemyStates::ES_DEFAULT:
	case EEnemyStates::ES_ENTRY_AMOUNT:
	default:
		break;
	}
}

// Called to bind functionality to input
void AAyoWhoDis::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAyoWhoDis::AttackRange_BeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
	UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if (AProjectWonkyCharacter* player = Cast<AProjectWonkyCharacter>(_otherActor))
	{
		SetCurrentState(EEnemyStates::ES_Attack);

		world->GetTimerManager().SetTimer(cooldownTimerHandle, this, &AAyoWhoDis::CommitAttack, attackCooldown);

		aiController->StopMovement();
	}
}

void AAyoWhoDis::AttackRange_EndOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
	UPrimitiveComponent* _otherComp, int32 _otherBodyIndex)
{
	// Wenn der Player aus der attack range raus is soll der enemy nach ablauf des timer, bestimmt durch seinen attack cooldown, den spieler verfolgen
	if (AProjectWonkyCharacter* player = Cast<AProjectWonkyCharacter>(_otherActor))
	{
		EEnemyStates newstate = EEnemyStates::ES_MoveToTarget;

		auto waitlambda = [this, newstate]()
			{
				SetCurrentState(newstate);
			};

		FTimerDelegate timerdele;
		timerdele.BindLambda(waitlambda);

		world->GetTimerManager().SetTimer(cooldownTimerHandle, timerdele, attackCooldown, false);
	}
}

void AAyoWhoDis::AggroRange_BeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
	UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if (AProjectWonkyCharacter* player = Cast<AProjectWonkyCharacter>(_otherActor))
	{
		targetPlayer = player;
		SetCurrentState(EEnemyStates::ES_MoveToTarget);
	}
}

void AAyoWhoDis::AggroRange_EndOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
	UPrimitiveComponent* _otherComp, int32 _otherBodyIndex)
{
	if (AProjectWonkyCharacter* player = Cast<AProjectWonkyCharacter>(_otherActor))
	{
		targetPlayer = nullptr;
		SetCurrentState(EEnemyStates::ES_Idle);
	}
}

void AAyoWhoDis::OnEnemyDeath()
{
	world->GetTimerManager().SetTimer(ragdollTimerHandle, this, &AAyoWhoDis::CleanEnemyDeath, ragdolltimer);
}

void AAyoWhoDis::CleanEnemyDeath()
{
	this->Destroy();
}

void AAyoWhoDis::Enemy_TakeDamage(float _damage, FVector _knockback)
{
	// Apply Velo here, die gegner sollen auch velo erhalten k�nnen wenn sie tot sind 

	if (currentHealth - _damage <= 0)
	{
		OnEnemyDeath();
	}
	else
	{
		currentHealth -= _damage;
	}
}

void AAyoWhoDis::CommitAttack()
{
	if (!targetPlayer)
		return;

	FVector launchvelo = FVector(0, attackKnockback, attackKnockback);

	targetPlayer->LaunchCharacter(launchvelo, true, false);

	// Give Damage to players
}

void AAyoWhoDis::SetCurrentState(EEnemyStates _newState)
{
	currentState = _newState;
}

void AAyoWhoDis::State_MoveToTarget()
{
	// Target kann eh nur player sein

	if (!targetPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("AEnemyBase, !targetPlayer"));
		return;
	}

	if (!aiController)
	{
		UE_LOG(LogTemp, Warning, TEXT("AEnemyBase, !aiController"));
		return;
	}

	//FVector adjplayerpos = FVector(targetPlayer->GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);

	//aiController->MoveToLocation(targetPlayer->GetActorLocation());
	aiController->MoveToActor(targetPlayer);
}

void AAyoWhoDis::State_Idle()
{
	aiController->MoveToLocation(startPos);
}

void AAyoWhoDis::State_AttackCache()
{
	// Do Jack Shit
}
