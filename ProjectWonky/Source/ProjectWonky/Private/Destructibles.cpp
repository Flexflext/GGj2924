// Fill out your copyright notice in the Description page of Project Settings.


#include "Destructibles.h"

#include "ThrowableObject.h"

// Sets default values
ADestructibles::ADestructibles()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	objectMesh = CreateDefaultSubobject<UStaticMeshComponent>("Object Mesh");
	RootComponent = objectMesh;
}

// Called when the game starts or when spawned
void ADestructibles::BeginPlay()
{
	Super::BeginPlay();

	world = GetWorld();
}

// Called every frame
void ADestructibles::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADestructibles::Destructible_TakeDamage(float _damage)
{
	if (destructibleHealth - _damage <= 0)
	{
		OnDestructibleDeath();
	}
	else
		destructibleHealth -= _damage;
}

void ADestructibles::OnDestructibleDeath()
{
	world->GetTimerManager().SetTimer(deathTimerHandle, this, &ADestructibles::CleanDestructibleDeath, deathTimer, false);
}

void ADestructibles::CleanDestructibleDeath()
{
	this->Destroy();
}

