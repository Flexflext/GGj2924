// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrowableObject.generated.h"

UCLASS()
class PROJECTWONKY_API AThrowableObject : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AThrowableObject();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DefaultSetup, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* hitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DefaultSetup, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* mesh;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DefaultSetup, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* holdingPosition;





protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
	FVector GetHoldingPosition();

};
