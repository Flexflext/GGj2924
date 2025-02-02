// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumLibrary.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

UCLASS()
class PROJECTWONKY_API AEnemyBase : public ACharacter
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

	UFUNCTION()
	void FadeMaterial(float _dt);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = EnemyStats, meta = (AllowPrivateAccess))
	float enemyMaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = EnemyStats, meta = (AllowPrivateAccess))
	float currentHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = EnemyStatus, meta = (AllowPrivateAccess))
	bool bHasDied;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	class USphereComponent* attackBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	USphereComponent* aggroBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyStatus, meta = (AllowPrivateAccess))
	class AProjectWonkyCharacter* targetPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	USoundBase* takeDamage_SFX;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	USoundBase* fart_SFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	TArray<USoundBase*> expressionSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	class UNiagaraComponent* niagaraComp;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category= EnemyStats , meta=(AllowPrivateAccess))
	float currentRagdolltimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	bool bActivateRagdoll;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	float attackKnockback;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	bool bIsStaggered;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	EEnemyStates currentState;

	UPROPERTY()
	UWorld* world;

	UPROPERTY()
	FVector startPos;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	float enemyDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	float yDefault;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	class AAIController* aiController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	float materialDefaultValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	bool bIsMoving;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	bool bCanAttackPlayer;
private:

	UFUNCTION()
	virtual void AttackRange_BeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult);
	UFUNCTION()
	virtual void AttackRange_EndOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex);

	UFUNCTION()
	virtual void AggroRange_BeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult);
	UFUNCTION()
	virtual void AggroRange_EndOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex);

	UFUNCTION()
	void OnEnemyDeath(FVector _knockback);

	UFUNCTION()
	void CleanEnemyDeath();

	UFUNCTION()
	void CommitAttack();
	UFUNCTION()
	void ResetGlobalTimeDilation();

	UFUNCTION()
	void SetCurrentState(EEnemyStates _newState);

	UFUNCTION()
	void State_MoveToTarget();

	UFUNCTION()
	void State_Idle();
	UFUNCTION()
	void State_Staggered();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	float attackCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnemyStats, meta = (AllowPrivateAccess))
	float maxAttackCooldown;

	UFUNCTION()
	void ResetStaggered();

	UFUNCTION()
	void TickAttack(float _dt);
	UFUNCTION()
	void TickRagdoll(float _dt);

public:
	UFUNCTION()
		void Enemy_TakeDamage(float _damage, FVector _knockback);

	UFUNCTION(BlueprintCallable) FORCEINLINE
	bool GetIsMoving() { return bIsMoving; }
};
