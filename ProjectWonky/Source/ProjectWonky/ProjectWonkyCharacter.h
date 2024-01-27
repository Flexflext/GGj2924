// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ProjectWonkyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AProjectWonkyCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* sphereAttackCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* sphereThrowableObject;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* holdingPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* arrowPosition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* throwIndicatorArrow;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;


	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PunchAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PickupAction;
	///** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;



public:
	AProjectWonkyCharacter();
	
	UFUNCTION()
	void Player_TakeDamage(float _damage);
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Attack(const FInputActionValue& Value);


	

	UFUNCTION()
	void OnPlayerDeath();

	

	UFUNCTION()
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void PickupObject(const FInputActionValue& Value);


	UFUNCTION()
	void JumpStart(const FInputActionValue& Value);

	UFUNCTION()
	void JumpEnd();

	UFUNCTION()
	void RegenAttack();

	UFUNCTION()
	void RegenFootStep();

	UFUNCTION()
	void Throw();

	UFUNCTION()
	void DelayedAttack();

	UFUNCTION()
	virtual void AttackRange_EndOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex);
	UFUNCTION()
	virtual void AttackRange_BeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult);


	UFUNCTION()
	virtual void PickUpRange_EndOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex);
	UFUNCTION()
	virtual void PickUpRange_BeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();


	virtual void Tick(float DeltaSeconds) override;

protected:

	UPROPERTY()
	class AThrowableObject* objectInRange;

	UPROPERTY()
	class AThrowableObject* throwObject;


	UPROPERTY()
	class AEnemyBase* enemyToAttack;
	UPROPERTY()
	class ADestructibles* currDestructible;

	UPROPERTY()
	bool holdingObject;

	UPROPERTY()
	bool canAttackEnemy;

	UPROPERTY()
	bool inAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float attackCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float throwforce;;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float rotationAmount;;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float playerHealth;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float playerMaxHealth;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float meeleDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float meeleknockbackForce;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	FVector2D minMaxClampRotation;;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float vectorRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Walking, meta = (AllowPrivateAccess = true))
	float timebetweenFootSteps = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float timeDelayforPunch = 0.5f;

	UPROPERTY()
	bool onAttackCooldown;


	UPROPERTY()
	bool wasfalling;

	UPROPERTY()
	FTimerHandle attackCoolDownHandle;

	UPROPERTY()
	FTimerHandle playNewSoundTImer;

	UPROPERTY()
	FTimerHandle attackDelayTimer;

	UPROPERTY()
	bool canPlayNewSound;

	UPROPERTY()
	UWorld* world;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sounds, meta = (AllowPrivateAccess = true))
	class USoundBase* getdamageSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sounds, meta = (AllowPrivateAccess = true))
	class USoundBase* punchSound;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sounds, meta = (AllowPrivateAccess = true))
	class USoundBase* pickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sounds, meta = (AllowPrivateAccess = true))
	class USoundBase* landSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sounds, meta = (AllowPrivateAccess = true))
	class USoundBase* jumpSound;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sounds, meta = (AllowPrivateAccess = true))
	class USoundBase* attackSound;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sounds, meta = (AllowPrivateAccess = true))
	TArray<class USoundBase*> throwSounds;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sounds, meta = (AllowPrivateAccess = true))
	TArray<class USoundBase*> footSteps;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = true))
	class UAnimMontage* attackAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = true))
	class UAnimMontage* pickupAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = true))
	class UAnimMontage* throwAnimation;



public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE
	float GetPlayerHealth() { return playerHealth; }
	FORCEINLINE
	float GetPlayerMaxHealth() { return playerMaxHealth; }

};

