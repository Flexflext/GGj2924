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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* hitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* mesh;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* holdingPosition;



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* arrowPosition;


private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	float knockbackForce;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	float damage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	float damageVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	int uses;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	bool bIsUsed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	bool bIsPicketUp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sounds, meta = (AllowPrivateAccess = true))
	class USoundBase* impactSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sounds, meta = (AllowPrivateAccess = true))
	float verticalAdditive;

	UPROPERTY()
	FTimerHandle destroyTimerHandle;

	UPROPERTY()
	FTimerHandle soundTimerHandle;

	UPROPERTY()
	bool canplaySound;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void Hit_BeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult);
	UFUNCTION()
	virtual void Hit_EndOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex);


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Damit der würfel auch irgendwann aufhört damagae zu machen nach dem werfen, falls nichts gehittet wurde
	UFUNCTION()
	void TickUsedStatus();

	FORCEINLINE
		void SetItemPickStatus(bool _status) { bIsPicketUp = _status; if (bIsPicketUp) { bIsUsed = true; } }

	UFUNCTION()
	FVector GetHoldingPosition();

	UFUNCTION()
	USceneComponent* GetArrowPosition();


	UFUNCTION()
	void DestroySoon();


	UFUNCTION()
	void CanPlayNewSound();


};
