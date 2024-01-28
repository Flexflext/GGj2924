// Fill out your copyright notice in the Description page of Project Settings.


#include "WinConditionManager.h"

#include "Kismet/GameplayStatics.h"
#include "ProjectWonky/ProjectWonkyCharacter.h"

// Sets default values
AWinConditionManager::AWinConditionManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	winCollider = CreateDefaultSubobject<UStaticMeshComponent>("WinCollider");
	RootComponent = winCollider;
}

// Called when the game starts or when spawned
void AWinConditionManager::BeginPlay()
{
	Super::BeginPlay();


	winCollider->OnComponentBeginOverlap.AddDynamic(this, &AWinConditionManager::OnPlayer_BeginOverlap);
}

void AWinConditionManager::OnPlayer_BeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
	UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if(AProjectWonkyCharacter* player  =Cast<AProjectWonkyCharacter>(_otherActor))
	{
		bGameWon = true;
	}
}

// Called every frame
void AWinConditionManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWinConditionManager::DeductPlayerLife()
{
	playerLifes--;

	if (playerLifes <= 0)
	{
		bGameLost = true;
		OnDeathEvent();
	}
	//UGameplayStatics::PlaySound2D()

	//UGameplayStatics::Sound
}

