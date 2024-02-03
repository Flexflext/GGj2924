// Fill out your copyright notice in the Description page of Project Settings.


#include "Destructibles.h"

#include "ThrowableObject.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADestructibles::ADestructibles()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	rootMesh = CreateDefaultSubobject<UStaticMeshComponent>("Root Mesh");
	RootComponent = rootMesh;

	objectMesh_0 = CreateDefaultSubobject<UStaticMeshComponent>("Object Mesh 0");

	objectMesh_1 = CreateDefaultSubobject<UStaticMeshComponent>("Object Mesh 1");

	objectMesh_2 = CreateDefaultSubobject<UStaticMeshComponent>("Object Mesh 2");

	objectMesh_3 = CreateDefaultSubobject<UStaticMeshComponent>("Object Mesh 3");

	objectMesh_4 = CreateDefaultSubobject<UStaticMeshComponent>("Object Mesh 4");

	objectMesh_5 = CreateDefaultSubobject<UStaticMeshComponent>("Object Mesh 5");

	objectMesh_6 = CreateDefaultSubobject<UStaticMeshComponent>("Object Mesh 6");

	objectMesh_7 = CreateDefaultSubobject<UStaticMeshComponent>("Object Mesh 7");

	objectMesh_8 = CreateDefaultSubobject<UStaticMeshComponent>("Object Mesh 8");

	objectMesh_9 = CreateDefaultSubobject<UStaticMeshComponent>("Object Mesh 9");

	objectMesh_10 = CreateDefaultSubobject<UStaticMeshComponent>("Object Mesh 10");

	objectMesh_11 = CreateDefaultSubobject<UStaticMeshComponent>("Object Mesh 11");

	objectMesh_12 = CreateDefaultSubobject<UStaticMeshComponent>("Object Mesh 12");

	objectMesh_13 = CreateDefaultSubobject<UStaticMeshComponent>("Object Mesh 13");

	objectMesh_14 = CreateDefaultSubobject<UStaticMeshComponent>("Object Mesh 14");

	objectMesh_0->SetupAttachment(GetRootComponent());
	objectMesh_1->SetupAttachment(GetRootComponent());
	objectMesh_2->SetupAttachment(GetRootComponent());
	objectMesh_3->SetupAttachment(GetRootComponent());
	objectMesh_4->SetupAttachment(GetRootComponent());
	objectMesh_5->SetupAttachment(GetRootComponent());
	objectMesh_6->SetupAttachment(GetRootComponent());
	objectMesh_7->SetupAttachment(GetRootComponent());
	objectMesh_8->SetupAttachment(GetRootComponent());
	objectMesh_9->SetupAttachment(GetRootComponent());
	objectMesh_10->SetupAttachment(GetRootComponent());
	objectMesh_11->SetupAttachment(GetRootComponent());
	objectMesh_12->SetupAttachment(GetRootComponent());
	objectMesh_13->SetupAttachment(GetRootComponent());
	objectMesh_14->SetupAttachment(GetRootComponent());


	materialDefaultValue = -1.f;

	allMeshes.Add(objectMesh_0);
	allMeshes.Add(objectMesh_1);
	allMeshes.Add(objectMesh_2);
	allMeshes.Add(objectMesh_3);
	allMeshes.Add(objectMesh_4);
	allMeshes.Add(objectMesh_5);
	allMeshes.Add(objectMesh_6);
	allMeshes.Add(objectMesh_7);
	allMeshes.Add(objectMesh_8);
	allMeshes.Add(objectMesh_9);
	allMeshes.Add(objectMesh_10);
	allMeshes.Add(objectMesh_11);
	allMeshes.Add(objectMesh_12);
	allMeshes.Add(objectMesh_13);
	allMeshes.Add(objectMesh_14);
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

void ADestructibles::Destructible_TakeDamage(float _damage, FVector _knockback)
{
	if (destructibleHealth - _damage <= 0)
	{
		OnDestructibleDeath(_knockback);
	}
	else
	{
		UGameplayStatics::PlaySound2D(GetWorld(), damage_sfx);
		destructibleHealth -= _damage;
	}
}

void ADestructibles::TickMaterialFade(float _dt)
{
	float fade = materialDefaultValue += _dt * 2;

	for (UStaticMeshComponent* mesh : allMeshes)
	{
		mesh->SetScalarParameterValueOnMaterials("FadeAmount", fade);
	}

	if (fade >= 1.f)
		CleanDestructibleDeath();
}

void ADestructibles::OnDestructibleDeath(FVector _knockback)
{
	bHasDied = true;

	for (UStaticMeshComponent* mesh : allMeshes)
	{
		mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		mesh->SetSimulatePhysics(true);

		mesh->AddForce(_knockback / 10);
	}

	UGameplayStatics::PlaySound2D(GetWorld(), death_sfx,1, 1, sfxStartPos);
}

void ADestructibles::CleanDestructibleDeath()
{
	this->Destroy();
}

