// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectWonkyCharacter.h"

#include "Destructibles.h"
#include "EnemyBase.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ThrowableObject.h"
#include "WinConditionManager.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AProjectWonkyCharacter

AProjectWonkyCharacter::AProjectWonkyCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm



	sphereAttackCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	sphereAttackCollision->SetupAttachment(RootComponent);


	sphereThrowableObject = CreateDefaultSubobject<USphereComponent>(TEXT("SphereThrowable"));
	sphereThrowableObject->SetupAttachment(RootComponent);


	holdingPosition = CreateDefaultSubobject<USceneComponent>(TEXT("HoldingPosition"));
	holdingPosition->SetupAttachment(RootComponent);

	arrowPosition = CreateDefaultSubobject<USceneComponent>(TEXT("ArrowPosition"));
	arrowPosition->SetupAttachment(holdingPosition);

	throwIndicatorArrow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ThrowIndiArrow"));
	throwIndicatorArrow->SetupAttachment(arrowPosition);
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)


	playerHealth = playerMaxHealth;
	knockbackAdditive = 1.5f;
}

void AProjectWonkyCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	world = GetWorld();

	canPlayNewSound = true;
	throwIndicatorArrow->SetHiddenInGame(true);

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (sphereAttackCollision != nullptr)
	{
		//sphereAttackCollision->OnComponentBeginOverlap.AddDynamic(this,TEXT("AttackRange_BeginOverlap"));

		sphereAttackCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectWonkyCharacter::AttackRange_BeginOverlap);
		sphereAttackCollision->OnComponentEndOverlap.AddDynamic(this, &AProjectWonkyCharacter::AttackRange_EndOverlap);
	}

	if (sphereThrowableObject != nullptr)
	{
		sphereThrowableObject->OnComponentBeginOverlap.AddDynamic(this, &AProjectWonkyCharacter::PickUpRange_BeginOverlap);
		sphereThrowableObject->OnComponentEndOverlap.AddDynamic(this, &AProjectWonkyCharacter::PickUpRange_EndOverlap);
	}
}

void AProjectWonkyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (wasfalling && GetCharacterMovement()->IsMovingOnGround())
	{
		JumpEnd();
	}


	wasfalling = GetCharacterMovement()->IsFalling();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AProjectWonkyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AProjectWonkyCharacter::JumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProjectWonkyCharacter::Move);

		EnhancedInputComponent->BindAction(PunchAction, ETriggerEvent::Started, this, &AProjectWonkyCharacter::Attack);
		EnhancedInputComponent->BindAction(PickupAction, ETriggerEvent::Started, this, &AProjectWonkyCharacter::PickupObject);
		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProjectWonkyCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AProjectWonkyCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);

		if (!inAir)
		{
			if (GetVelocity().X != 0)
			{
				if (canPlayNewSound)
				{

					canPlayNewSound = false;
					world->GetTimerManager().SetTimer(
						playNewSoundTImer,
						this,
						&AProjectWonkyCharacter::RegenFootStep,
						timebetweenFootSteps,
						false);


					USoundBase* Sound = footSteps[FMath::RandRange(0, footSteps.Num() - 1)];

					if (Sound)
					{
						UGameplayStatics::PlaySound2D(world, Sound, 1);
					}
				}
			}
		}
		

		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AProjectWonkyCharacter::Attack(const FInputActionValue& Value)
{
	if (onAttackCooldown) return;

	onAttackCooldown = true;
	world->GetTimerManager().SetTimer(
		attackCoolDownHandle, 
		this,
		&AProjectWonkyCharacter::RegenAttack, 
		attackCooldown, 
		false);


	if (holdingObject)
	{

		UE_LOG(LogTemp, Warning, TEXT("Throw"))
		Throw();
	}
	else
	{
		onAttackCooldown = true;

		UGameplayStatics::PlaySound2D(world, attackSound);

		world->GetTimerManager().SetTimer(
			attackDelayTimer,
			this,
			&AProjectWonkyCharacter::DelayedAttack,
			timeDelayforPunch,
			false);


		//Play Attack Animation
		UE_LOG(LogTemp, Warning, TEXT("AttackAnimation"));

		PlayAnimMontage(attackAnimMontage);

		
	}

	

	
}

void AProjectWonkyCharacter::Player_TakeDamage(float _damage)
{
	playerHealth -= _damage;

	UGameplayStatics::PlaySound2D(world, getdamageSound);

	if (playerHealth <= 0)
	{
		OnPlayerDeath();
		playerHealth = playerMaxHealth;
	}
}

void AProjectWonkyCharacter::OnPlayerDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Player is Dead"));

	if (AWinConditionManager* wcManager = Cast<AWinConditionManager>(UGameplayStatics::GetActorOfClass(world, AWinConditionManager::StaticClass())))
		wcManager->DeductPlayerLife();



}

void AProjectWonkyCharacter::PickupObject(const FInputActionValue& Value)
{
	if (holdingObject) return;
	if (objectInRange == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("PickUp"));

	throwObject = objectInRange;

	if (throwObject != nullptr)
	{
		vectorRotation = 0;
		throwObject->mesh->SetSimulatePhysics(false);


		PlayAnimMontage(pickupAnimation);


		UGameplayStatics::PlaySound2D(world, pickupSound);

		throwObject->SetActorRotation(holdingPosition->GetRelativeRotation(), ETeleportType::TeleportPhysics);

		throwObject->AttachToComponent(holdingPosition, FAttachmentTransformRules::KeepWorldTransform);


		throwObject->SetActorLocation(holdingPosition->GetComponentLocation() - throwObject->GetHoldingPosition());
		arrowPosition->SetWorldLocation(holdingPosition->GetComponentLocation() - throwObject->GetHoldingPosition());
		throwIndicatorArrow->SetRelativeLocation(throwObject->GetArrowPosition()->GetRelativeLocation());

		throwObject->SetItemPickStatus(true);

		throwIndicatorArrow->SetHiddenInGame(false);
	}

	holdingObject = true;
}

void AProjectWonkyCharacter::JumpStart(const FInputActionValue& Value)
{
	Jump();
	inAir = true;

	UGameplayStatics::PlaySound2D(world, jumpSound);
}

void AProjectWonkyCharacter::JumpEnd()
{
	inAir = false;

	UGameplayStatics::PlaySound2D(world, landSound);
}

void AProjectWonkyCharacter::RegenAttack()
{
	onAttackCooldown = false;
}

void AProjectWonkyCharacter::RegenFootStep()
{
	canPlayNewSound = true;
}

void AProjectWonkyCharacter::Throw()
{
	USoundBase* Sound = throwSounds[FMath::RandRange(0, throwSounds.Num() - 1)];

	if (Sound)
	{
		UGameplayStatics::PlaySound2D(world, Sound);
	}
	//Throw Object
		//Play Throw Animation

	//throwObject->DetachFromComponent
	PlayAnimMontage(throwAnimation);

	throwObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	throwIndicatorArrow->SetHiddenInGame(true);

	throwObject->mesh->SetSimulatePhysics(true);

	throwObject->SetItemPickStatus(false);

	throwObject->mesh->AddForce(throwObject->mesh->GetForwardVector() * throwforce, NAME_None, true);




	throwObject = nullptr;
	holdingObject = false;
}

void AProjectWonkyCharacter::DelayedAttack()
{
	FVector knockback = FVector(meeleknockbackForce, 0, meeleknockbackForce / 1.5);

	if (canAttackEnemy)
	{
		USoundBase* Sound = punchSounds[FMath::RandRange(0, punchSounds.Num() - 1)];

		if (Sound)
		{
			UGameplayStatics::PlaySound2D(world, Sound);
		}

		UE_LOG(LogTemp, Warning, TEXT("Attack"))
			//Attack Enemy here
		if (enemyToAttack->GetActorLocation().X < GetActorLocation().X)
		{
			knockback = FVector(-meeleknockbackForce, 0, meeleknockbackForce / 1.5);
		}

		OnPunch(enemyToAttack->GetActorLocation());
		knockback.Z *= knockbackAdditive;
		enemyToAttack->Enemy_TakeDamage(meeleDamage, knockback);

	}

	if (currDestructible && !currDestructible->GetHasDied())
		currDestructible->Destructible_TakeDamage(meeleDamage, knockback);
}

void AProjectWonkyCharacter::AttackRange_EndOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
                                                    UPrimitiveComponent* _otherComp, int32 _otherBodyIndex)
{
	if (_otherComp->ComponentHasTag(FName("Enemy")))
	{
		canAttackEnemy = false;
		enemyToAttack = nullptr;
	}
}

void AProjectWonkyCharacter::AttackRange_BeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
	UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if (_otherComp->ComponentHasTag(FName("Enemy")))
	{
		enemyToAttack = Cast<AEnemyBase>(_otherActor);

		if (enemyToAttack)
		{
			canAttackEnemy = true;
		}
		
	}

	if (ADestructibles* destructibles = Cast<ADestructibles>(_otherActor))
	{
		currDestructible = destructibles;
	}

}

void AProjectWonkyCharacter::PickUpRange_EndOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
	UPrimitiveComponent* _otherComp, int32 _otherBodyIndex)
{
	if (objectInRange != nullptr)
	{
		objectInRange = nullptr;

		//UE_LOG(LogTemp, Warning, TEXT("PickUpOutRange"))
	}
}

void AProjectWonkyCharacter::PickUpRange_BeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
	UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if (_otherComp->ComponentHasTag(FName("Throwable")))
	{
		objectInRange = Cast<AThrowableObject>(_otherActor);
		
		//UE_LOG(LogTemp, Warning, TEXT("PickUpInRange"))

		if (objectInRange == nullptr)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Throwobj not converted although right tag"))
		}
	}

}



void AProjectWonkyCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		//AddControllerYawInput(LookAxisVector.X);
		//AddControllerPitchInput(LookAxisVector.Y);

		vectorRotation += LookAxisVector.Y * rotationAmount;

		vectorRotation = FMath::Clamp(vectorRotation, minMaxClampRotation.X, minMaxClampRotation.Y);



		if (throwObject != nullptr)
		{

			FRotator ror = { -vectorRotation,0,0 };
			//FRotator rorIndicator = { -vectorRotation - 90,0,0 };
			//ror += GetActorRotation();
			//rorIndicator += GetActorRotation();

			//dir = ror.RotateVector(dir);
			throwObject->SetActorRelativeRotation(ror); //, ETeleportType::TeleportPhysics);
			arrowPosition->SetRelativeRotation(ror);// , ETeleportType::TeleportPhysics);
		}

	}	
}