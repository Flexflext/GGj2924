// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Destructibles.generated.h"

UCLASS()
class PROJECTWONKY_API ADestructibles : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADestructibles();

protected:
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Destructible_TakeDamage(float _damage);
private:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Comps,meta=(AllowPrivateAccess))
	UStaticMeshComponent* objectMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess))
	float destructibleHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess))
	float deathTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess))
	FTimerHandle deathTimerHandle;

	UPROPERTY()
	UWorld* world;

private:

	UFUNCTION()
	void OnDestructibleDeath();

	UFUNCTION()
	void CleanDestructibleDeath();
};
