// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#include "DrawDebugHelpers.h"
#include "CodeTestCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "TimerManager.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ACodeTestCharacter

ACodeTestCharacter::ACodeTestCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ACodeTestCharacter::OnHit);
	// Set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Set default pickup delay (in seconds)
	PickupDelay = 1.0f;

}

void ACodeTestCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	Mesh1P->SetHiddenInGame(true, false);
}

void ACodeTestCharacter::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// attach weapon to the character if we hit an equippable weapon
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherActor->ActorHasTag("Equipable_Wep"))
	{
		if (!GetWorldTimerManager().IsTimerActive(PickupTimer))
		{
			OtherComp->BodyInstance.SetCollisionProfileName("NoCollision");
			OtherComp->SetSimulatePhysics(false);
			HeldWeapon = static_cast<AWeapon*>(OtherActor);
			HeldWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
			HeldWeapon->SetOwner(this);
			Mesh1P->SetHiddenInGame(false, false);
			GetWorldTimerManager().ClearTimer(PickupTimer);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ACodeTestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Primary Fire", IE_Pressed, this, &ACodeTestCharacter::OnPrimaryFire);

	// Bind grab event
	PlayerInputComponent->BindAction("Secondary Fire", IE_Pressed, this, &ACodeTestCharacter::OnSecondaryFire);

	// Bind drop event
	PlayerInputComponent->BindAction("Drop Weapon", IE_Pressed, this, &ACodeTestCharacter::OnDrop);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ACodeTestCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACodeTestCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACodeTestCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACodeTestCharacter::LookUpAtRate);
}

//Triggers on Primary fire
void ACodeTestCharacter::OnPrimaryFire()
{
	if (HeldWeapon != nullptr)
	{
		if (HeldWeapon->FirePrimary(FirstPersonCameraComponent))
		{
			// try and play a firing animation if specified
			if (FireAnimation != NULL)
			{
				// Get the animation object for the arms mesh
				UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
				if (AnimInstance != NULL)
				{
					AnimInstance->Montage_Play(FireAnimation, 1.f);
				}
			}
		}
	}
}

//Triggers on Secondary fire 
void ACodeTestCharacter::OnSecondaryFire()
{
	if (HeldWeapon != nullptr)
	{
		if (HeldWeapon->FireSecondary(FirstPersonCameraComponent))
		{
			// try and play a firing animation if specified
			if (FireAnimation != NULL)
			{
				// Get the animation object for the arms mesh
				UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
				if (AnimInstance != NULL)
				{
					AnimInstance->Montage_Play(FireAnimation, 1.f);
				}
			}
		}
	}
}

//Triggers when dropping weapons
void ACodeTestCharacter::OnDrop()
{
	GetWorldTimerManager().SetTimer(PickupTimer, PickupDelay, false);
	HeldWeapon->Dropped(FirstPersonCameraComponent);
	HeldWeapon = nullptr;
	Mesh1P->SetHiddenInGame(true, false);
	
}

void ACodeTestCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ACodeTestCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ACodeTestCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACodeTestCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
