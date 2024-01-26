// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "ProjectWonkyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	currentHealth = enemyMaxHealth;

	enemyHitbox = CreateDefaultSubobject<UCapsuleComponent>("Hitbox;");
	RootComponent = enemyHitbox;
	attackBox = CreateDefaultSubobject<USphereComponent>("AttackBox;");
	attackBox->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	world = GetWorld();

	enemyHitbox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::AEnemyBase::AttackRange_BeginOverlap);
	enemyHitbox->OnComponentEndOverlap.AddDynamic(this, &AEnemyBase::AttackRange_EndOverlap);
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::AttackRange_BeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
	UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if (AProjectWonkyCharacter* player = Cast<AProjectWonkyCharacter>(_otherActor))
	{
		targetPlayer = player;

		world->GetTimerManager().SetTimer(cooldownTimerHandle, this, &AEnemyBase::CommitAttack, attackCooldown);
	}
}

void AEnemyBase::AttackRange_EndOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
	UPrimitiveComponent* _otherComp, int32 _otherBodyIndex)
{
}

void AEnemyBase::OnEnemyDeath()
{
	world->GetTimerManager().SetTimer(ragdollTimerHandle, this, &AEnemyBase::CleanEnemyDeath, ragdolltimer);
}

void AEnemyBase::CleanEnemyDeath()
{
	this->Destroy();
}

void AEnemyBase::Enemy_TakeDamage(float _damage, FVector _knockback)
{
	// Apply Velo here, die gegner sollen auch velo erhalten können wenn sie tot sind 

	if(currentHealth - _damage <= 0)
	{
		OnEnemyDeath();
	}
	else
	{
		currentHealth -= _damage;
	}
}

void AEnemyBase::CommitAttack()
{
	if (!targetPlayer)
		return;

	FVector launchvelo = FVector(0, attackKnockback, attackKnockback);

	targetPlayer->LaunchCharacter(launchvelo, true,false);

	// Give Damage to players
}

