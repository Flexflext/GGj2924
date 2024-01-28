// Fill out your copyright notice in the Description page of Project Settings.


#include "Destructibles.h"

#include "ThrowableObject.h"
#include "Kismet/GameplayStatics.h"

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

	if (bHasDied)
		TickMaterialFade(DeltaTime);
}

void ADestructibles::Destructible_TakeDamage(float _damage)
{
	if (destructibleHealth - _damage <= 0)
	{
		OnDestructibleDeath();
	}
	else
	{
		UGameplayStatics::PlaySound2D(GetWorld(), damage_sfx);
		destructibleHealth -= _damage;
	}
}

void ADestructibles::TickMaterialFade(float _dt)
{
	objectMesh->SetScalarParameterValueOnMaterials("FadeAmount", materialDefaultValue += _dt);
}

void ADestructibles::OnDestructibleDeath()
{
	bHasDied = true;

	objectMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UGameplayStatics::PlaySound2D(GetWorld(), death_sfx);

	world->GetTimerManager().SetTimer(deathTimerHandle, this, &ADestructibles::CleanDestructibleDeath, deathTimer, false);
}

void ADestructibles::CleanDestructibleDeath()
{
	this->Destroy();
}

