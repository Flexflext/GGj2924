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
	void Destructible_TakeDamage(float _damage, FVector _knockback);

	FORCEINLINE
		bool GetHasDied() { return bHasDied; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	UStaticMeshComponent* rootMesh;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Comps,meta=(AllowPrivateAccess))
	UStaticMeshComponent* objectMesh_0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	UStaticMeshComponent* objectMesh_1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	UStaticMeshComponent* objectMesh_2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	UStaticMeshComponent* objectMesh_3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	UStaticMeshComponent* objectMesh_4;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	UStaticMeshComponent* objectMesh_5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	UStaticMeshComponent* objectMesh_6;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	UStaticMeshComponent* objectMesh_7;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	UStaticMeshComponent* objectMesh_8;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	UStaticMeshComponent* objectMesh_9;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	UStaticMeshComponent* objectMesh_10;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	UStaticMeshComponent* objectMesh_11;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	UStaticMeshComponent* objectMesh_12;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	UStaticMeshComponent* objectMesh_13;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	UStaticMeshComponent* objectMesh_14;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	TArray<UStaticMeshComponent*> allMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess))
	float destructibleHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess))
	USoundBase* damage_sfx;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess))
	USoundBase* death_sfx;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess))
	float sfxStartPos;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess))
	FTimerHandle deathTimerHandle;

	UPROPERTY()
	UWorld* world;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess))
	bool bHasDied;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess))
	float materialDefaultValue;

	UFUNCTION()
	void TickMaterialFade(float _dt);

private:

	UFUNCTION()
	void OnDestructibleDeath(FVector _knockback);

	UFUNCTION()
	void CleanDestructibleDeath();
};
