// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WinConditionManager.generated.h"

UCLASS()
class PROJECTWONKY_API AWinConditionManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWinConditionManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnPlayer_BeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void DeductPlayerLife();

private:
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	UStaticMeshComponent* winCollider;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	int playerLifes;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	bool bGameWon;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	bool bGameLost;
};
