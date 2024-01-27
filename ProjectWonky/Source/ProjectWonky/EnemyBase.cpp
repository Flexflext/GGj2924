// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "ProjectWonkyCharacter.h"
#include "Runtime/AIModule/Classes/AIController.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	currentHealth = enemyMaxHealth;

	aggroBox = CreateDefaultSubobject<USphereComponent>("AggroBox");
	aggroBox->SetupAttachment(GetRootComponent());

	attackBox = CreateDefaultSubobject<USphereComponent>("AttackBox;");
	attackBox->SetupAttachment(GetRootComponent());

	yDefault = 1000.f;

	materialDefaultValue = -1.f;

	attackCooldown = 0.4f;

	niagaraComp = CreateDefaultSubobject<UNiagaraComponent>("Niagara");
	niagaraComp->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	world = GetWorld();

	attackBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::AEnemyBase::AttackRange_BeginOverlap);
	attackBox->OnComponentEndOverlap.AddDynamic(this, &AEnemyBase::AttackRange_EndOverlap);

	aggroBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::AggroRange_BeginOverlap);
	aggroBox->OnComponentEndOverlap.AddDynamic(this, &AEnemyBase::AggroRange_EndOverlap);
	
	aiController = Cast<AAIController>(GetController());
	currentHealth = enemyMaxHealth;

	startPos = GetActorLocation();

	PlayRandomExpression();
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHasDied)
	{
		FadeMaterial(DeltaTime);
		TickRagdoll(DeltaTime);
		return;
	}


	switch (currentState)
	{
		// Ich nutze Attack erstmal nur als empty weil ich return über idle regeln will und attack eh über den timer om overlap
	case EEnemyStates::ES_Attack:
		TickAttack(DeltaTime);
		break;
		// Idle soll die AI halt auch zu ihrer start pos zurück gehen lassen
	case EEnemyStates::ES_Idle:
		State_Idle();
		break;
	case EEnemyStates::ES_MoveToTarget:
		State_MoveToTarget();
		break;
	case EEnemyStates::ES_Staggered:
		State_Staggered();
		break;

	case EEnemyStates::ES_DEFAULT:
	case EEnemyStates::ES_ENTRY_AMOUNT:
		default:
		break;
	}

	if (GetVelocity().Length() <= 0)
		bIsMoving = false;
	else
		bIsMoving = true;
}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::FadeMaterial(float _dt)
{
	GetMesh()->SetScalarParameterValueOnMaterials("FadeAmount", materialDefaultValue += _dt);
}

void AEnemyBase::AttackRange_BeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
                                          UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if (bIsStaggered || bHasDied)
		return;

	if (AProjectWonkyCharacter* player = Cast<AProjectWonkyCharacter>(_otherActor))
	{
		bcanAttackPlayer = true;

		SetCurrentState(EEnemyStates::ES_Attack);

		aiController->StopMovement();
	}
}

void AEnemyBase::AttackRange_EndOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
	UPrimitiveComponent* _otherComp, int32 _otherBodyIndex)
{
	if (bIsStaggered || bHasDied)
	{
		targetPlayer = nullptr;
		return;
	}

	// Wenn der Player aus der attack range raus is soll der enemy nach ablauf des timer, bestimmt durch seinen attack cooldown, den spieler verfolgen
	if (AProjectWonkyCharacter* player = Cast<AProjectWonkyCharacter>(_otherActor))
	{
		bcanAttackPlayer = false;

		EEnemyStates newstate = EEnemyStates::ES_MoveToTarget;

		SetCurrentState(newstate);
	}
}

void AEnemyBase::AggroRange_BeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
	UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if (bIsStaggered || bHasDied)
		return;

	if (AProjectWonkyCharacter* player = Cast<AProjectWonkyCharacter>(_otherActor))
	{
		targetPlayer = player;
		SetCurrentState(EEnemyStates::ES_MoveToTarget);
	}
}

void AEnemyBase::AggroRange_EndOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
	UPrimitiveComponent* _otherComp, int32 _otherBodyIndex)
{
	if (bIsStaggered || bHasDied)
	{
		targetPlayer = nullptr;
		return;
	}

	if (AProjectWonkyCharacter* player = Cast<AProjectWonkyCharacter>(_otherActor))
	{
		targetPlayer = nullptr;
		SetCurrentState(EEnemyStates::ES_Idle);
	}
}

void AEnemyBase::OnEnemyDeath(FVector _knockback)
{
	bHasDied = true;

	aiController->StopMovement();

	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->AddForce(_knockback);
}

void AEnemyBase::CleanEnemyDeath()
{
	this->Destroy();
}

void AEnemyBase::Enemy_TakeDamage(float _damage, FVector _knockback)
{
	// Apply Velo here, die gegner sollen auch velo erhalten können wenn sie tot sind 
	aiController->StopMovement();

	UGameplayStatics::SetGlobalTimeDilation(world, 0.45);
	FTimerHandle handle;
	world->GetTimerManager().SetTimer(handle, this, &AEnemyBase::ResetGlobalTimeDilation, .1f, false);

	if(takeDamage_SFX)
		UGameplayStatics::PlaySound2D(world, takeDamage_SFX);

	currentHealth -= _damage;

	SetCurrentState(EEnemyStates::ES_Staggered);

	if (currentHealth <= 0)
		OnEnemyDeath(_knockback);
}

void AEnemyBase::CommitAttack()
{
	if (!targetPlayer || bHasDied || !bcanAttackPlayer )
		return;

	// Noch schauen das ich das von der rotatrion abhängig mache
	FVector launchvelo = FVector(-attackKnockback, 0, attackKnockback);

	targetPlayer->LaunchCharacter(launchvelo, true,true);

	niagaraComp->ActivateSystem();

	targetPlayer->Player_TakeDamage(enemyDamage);
}

void AEnemyBase::ResetGlobalTimeDilation()
{
	UGameplayStatics::SetGlobalTimeDilation(world, 1);
}

void AEnemyBase::SetCurrentState(EEnemyStates _newState)
{
	currentState = _newState;

	if (currentState != EEnemyStates::ES_Staggered)
		return;

	bIsStaggered = true;

	auto waitlambda = [this]()
	{
		ResetStaggered();
	};

	FTimerDelegate timerdele;
	timerdele.BindLambda(waitlambda);

	FTimerHandle handle;

	world->GetTimerManager().SetTimer(handle, timerdele, currentRagdolltimer, false);
}

void AEnemyBase::State_MoveToTarget()
{
	// Target kann eh nur player sein

	if(!targetPlayer)
		return;

	if (!aiController)
	{
		UE_LOG(LogTemp, Warning, TEXT("AEnemyBase, !aiController"));
		return;
	}

	FVector adjplayerpos = FVector(targetPlayer->GetActorLocation().X, yDefault, GetActorLocation().Z);

	aiController->MoveToLocation(adjplayerpos);
}

void AEnemyBase::State_Idle()
{
	aiController->MoveToLocation(startPos);
}

void AEnemyBase::State_Staggered()
{
	aiController->StopMovement();
}

void AEnemyBase::ResetStaggered()
{
	bIsStaggered = false;

	if (targetPlayer)
		SetCurrentState(EEnemyStates::ES_MoveToTarget);
	else
		SetCurrentState(EEnemyStates::ES_Idle);
}

void AEnemyBase::PlayRandomExpression()
{
	if (bHasDied)
		return;

	int rnd = FMath::RandRange(0, expressionSounds.Num() - 1);
	UGameplayStatics::PlaySound2D(world, expressionSounds[rnd]);

	FTimerHandle handle;

	world->GetTimerManager().SetTimer(handle, this, &AEnemyBase::PlayRandomExpression, 5.f,false);
}

void AEnemyBase::TickAttack(float _dt)
{
	if (attackCooldown <= 0)
	{
		attackCooldown = maxAttackCooldown;
		CommitAttack();
	}
	else
		attackCooldown -= _dt;
}

void AEnemyBase::TickRagdoll(float _dt)
{
	if (currentRagdolltimer <= 0)
	{
		CleanEnemyDeath();
	}
	else
		currentRagdolltimer -= _dt;
}
