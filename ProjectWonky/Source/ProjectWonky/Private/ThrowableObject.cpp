// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowableObject.h"

#include "Components/BoxComponent.h"
#include "ProjectWonky/EnemyBase.h"

// Sets default values
AThrowableObject::AThrowableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mesh->SetupAttachment(RootComponent);


	holdingPosition = CreateDefaultSubobject<USceneComponent>(TEXT("HoldingPosition"));
	holdingPosition->SetupAttachment(RootComponent);

	hitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	hitBox->SetupAttachment(mesh);
	

}

// Called when the game starts or when spawned
void AThrowableObject::BeginPlay()
{
	Super::BeginPlay();

	this->Tags.AddUnique(FName("Throwable"));
	mesh->ComponentTags.AddUnique(FName("Throwable"));

	hitBox->OnComponentBeginOverlap.AddDynamic(this, &AThrowableObject::Hit_BeginOverlap);

}

void AThrowableObject::Hit_BeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
	UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if (!bIsUsed)
		return;

	if (_otherComp->ComponentHasTag("Enemy"))
	{
		AEnemyBase* enemy = Cast<AEnemyBase>(_otherActor);

		FVector knockback = FVector(knockbackForce, 0, knockbackForce / 4);
		if (enemy->GetActorLocation().X < GetActorLocation().X)
		{
			knockback = FVector(-knockbackForce, 0, knockbackForce / 4);
		}
		
		

		enemy->Enemy_TakeDamage(damage, knockback);

		uses--;
		if (uses <= 0)
		{
			GetWorld()->GetTimerManager().SetTimer(
				destroyTimerHandle,
				this,
				&AThrowableObject::DestroySoon,
				0.01f,
				false);

			
		}
		// Hier Animations oder so mit renne
	}
}

void AThrowableObject::Hit_EndOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
	UPrimitiveComponent* _otherComp, int32 _otherBodyIndex)
{
}

// Called every frame
void AThrowableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsPicketUp)
		TickUsedStatus();
}

void AThrowableObject::TickUsedStatus()
{
	if (GetVelocity().Length() <= damageVelocity)
	{
		bIsUsed = false;
	}
	else if (GetVelocity().Length() > damageVelocity)
		bIsUsed = true;
}

FVector AThrowableObject::GetHoldingPosition()
{
	return holdingPosition->GetRelativeLocation();
}

void AThrowableObject::DestroySoon()
{
	this->Destroy();
}

