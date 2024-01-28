// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowableObject.h"

#include "Destructibles.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
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

	arrowPosition = CreateDefaultSubobject<USceneComponent>(TEXT("ArrowPosition"));
	arrowPosition->SetupAttachment(RootComponent);

	hitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	hitBox->SetupAttachment(mesh);

	verticalAdditive = 1.5f;
}

// Called when the game starts or when spawned
void AThrowableObject::BeginPlay()
{
	Super::BeginPlay();
	canplaySound = true;
	this->Tags.AddUnique(FName("Throwable"));
	mesh->ComponentTags.AddUnique(FName("Throwable"));

	hitBox->OnComponentBeginOverlap.AddDynamic(this, &AThrowableObject::Hit_BeginOverlap);

}

void AThrowableObject::Hit_BeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
	UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if (!bIsUsed)
		return;

	if (canplaySound)
	{
		canplaySound = false;
		GetWorld()->GetTimerManager().SetTimer(
		soundTimerHandle,
			this,
			&AThrowableObject::CanPlayNewSound,
			0.3f,
			false);
		UGameplayStatics::PlaySound2D(GetWorld(), impactSound, 0.2f);
	}
	

	if (ADestructibles* destuctable = Cast<ADestructibles>(_otherActor))
	{
		destuctable->Destructible_TakeDamage(damage);
	}

	if (_otherComp->ComponentHasTag("Enemy"))
	{
		AEnemyBase* enemy = Cast<AEnemyBase>(_otherActor);

		FVector knockback = FVector(knockbackForce, 0, knockbackForce);
		if (enemy->GetActorLocation().X < GetActorLocation().X)
		{
			knockback = FVector(-knockbackForce, 0, knockbackForce);
		}
		
		knockback.Z *= verticalAdditive;

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

USceneComponent* AThrowableObject::GetArrowPosition()
{
	return arrowPosition;
}

void AThrowableObject::DestroySoon()
{
	this->Destroy();
}

void AThrowableObject::CanPlayNewSound()
{
	canplaySound = true;
}

