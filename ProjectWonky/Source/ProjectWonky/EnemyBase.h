// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyBase.generated.h"

UCLASS()
class PROJECTWONKY_API AEnemyBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE
		float GetCurrentHealth() { return currentHealth; }
	FORCEINLINE
		float GetMaxHealth() { return enemyMaxHealth; }
	FORCEINLINE
		bool GetHasDied() { return bHasDied; }


private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = EnemyStats, meta = (AllowPrivateAccess))
	float enemyMaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = EnemyStats, meta = (AllowPrivateAccess))
	float currentHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = EnemyStatus, meta = (AllowPrivateAccess))
	bool bHasDied;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = EnemyStats, meta = (AllowPrivateAccess))
	float attackRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	class USphereComponent* attackBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	class UCapsuleComponent* enemyHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyStatus, meta = (AllowPrivateAccess))
	class AProjectWonkyCharacter* targetPlayer;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=EnemyStatus, meta = (AllowPrivateAccess))
	FTimerHandle cooldownTimerHandle;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=EnemyStatus, meta = (AllowPrivateAccess))
	FTimerHandle ragdollTimerHandle;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category= EnemyStats , meta=(AllowPrivateAccess))
	float attackCooldown;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category= EnemyStats , meta=(AllowPrivateAccess))
	float ragdolltimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	float attackKnockback;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))


	UPROPERTY()
	UWorld* world;

private:
	UFUNCTION()
	virtual void AttackRange_BeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult);
	UFUNCTION()
	virtual void AttackRange_EndOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex);

	UFUNCTION()
	void OnEnemyDeath();

	UFUNCTION()
	void CleanEnemyDeath();

	UFUNCTION()
	void Enemy_TakeDamage(float _damage, FVector _knockback);

	UFUNCTION()
	void CommitAttack();


};
