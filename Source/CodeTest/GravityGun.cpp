// Fill out your copyright notice in the Description page of Project Settings.

#include "DrawDebugHelpers.h"
#include "GravityGun.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
// Sets default values for this component's properties
AGravityGun::AGravityGun()
{
	Tags.Add("Equipable_Wep");

	// Default offset from the character location for pulled objects to hover at
	MuzzleOffset = FVector(200.0f, 0.0f, 10.0f);
	FireRate = 0.4;
	PushMagnitude = 1000000.0f;
	TargetRange = 1000.0f;
	PullSpeedMultiplier = 1.0f;
	MassPullLimit = 1000.0f;
	// Set this component to be initialized when the game starts, and to be ticked every frame
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts
void AGravityGun::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void AGravityGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Check if currently holding an object
	if (Pulling)
	{
		if (LerpAlpha >= 1.0f)
		{
			//Update position of the held object
			PulledActor->SetActorLocation(Weapon_MuzzleLocation->GetComponentLocation() + static_cast<ACharacter*>(GetOwner())->GetControlRotation().RotateVector(MuzzleOffset));
			PulledComponent->SetPhysicsAngularVelocity(FVector(0.0f, 0.0f, 0.0f));
			PulledComponent->SetPhysicsLinearVelocity(FVector(0.0f, 0.0f, 0.0f));
		}
		else
		{
			//Continously pull towards weapon muzzle
			const FRotator SpawnRotation = static_cast<ACharacter*>(GetOwner())->GetControlRotation();
			
			PulledActor->SetActorLocation(FMath::Lerp(
				StartPullPos,
				Weapon_MuzzleLocation->GetComponentLocation() + SpawnRotation.RotateVector(MuzzleOffset),
				LerpAlpha));
			LerpAlpha += DeltaTime * PullSpeedMultiplier;
		}
	}
}


bool AGravityGun::FirePrimary(UCameraComponent * Camera)
{
	return Push(Camera);
}


bool AGravityGun::FireSecondary(UCameraComponent * Camera)
{
	return Pull(Camera);
}


void AGravityGun::Dropped(class UCameraComponent * Camera)
{
	Super::Dropped(Camera);
}


bool AGravityGun::Push(UCameraComponent * Camera)
{
	bool HitObject = false;
	
	if (!GetWorldTimerManager().IsTimerActive(FireRateTimer))
	{
		const FVector ForwardVector = Camera->GetForwardVector();
		// Check if currently holding an object
		if (Pulling)
		{
			// Let go and shot the held object
			PulledComponent->SetEnableGravity(true);
			PulledComponent->AddImpulseAtLocation(ForwardVector*PushMagnitude, Weapon_MuzzleLocation->GetComponentLocation());
			LerpAlpha = 0;
			PulledActor = nullptr;
			PulledComponent = nullptr;
			Pulling = false;
			HitObject = true;
			// Try and play the sound if specified
			if (FireSound != NULL)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetOwner()->GetActorLocation(), 0.5f);
			}
		}
		else
		{
			// Try and shot at a physics object
			FHitResult Output;
			const FVector Start = Weapon_MuzzleLocation->GetComponentLocation();
			const FVector End = ((ForwardVector * TargetRange) + Start);
			FCollisionQueryParams CollisionParams;
			if (GetWorld()->LineTraceSingleByChannel(Output, Start, End, ECC_Visibility, CollisionParams))
			{
				if (Output.bBlockingHit)
				{
					
					AActor* OtherActor = Output.GetActor();
					UPrimitiveComponent* OtherComp = Output.GetComponent();
					if ((OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
					{
						HitObject = true;
						//Push object away
						OtherComp->AddImpulseAtLocation(ForwardVector*PushMagnitude, Output.Location);
						// Try and play the sound if specified
						if (FireSound != NULL)
						{
							UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetOwner()->GetActorLocation(), 0.5f);
						}
					}
				}
			}
		}

		//Turn on timer for fire cooldown
		GetWorldTimerManager().SetTimer(FireRateTimer, FireRate, false);
	}

	return HitObject;
}

bool AGravityGun::Pull(class UCameraComponent* Camera)
{
	bool HitObject = false;
	// Check if currently holding an object
	if (Pulling)
	{
		//Drop the held object
		PulledComponent->SetEnableGravity(true);
		PulledComponent->SetPhysicsLinearVelocity(GetOwner()->GetVelocity() + Camera->GetForwardVector() * 10.0f);
		LerpAlpha = 0;
		PulledActor = nullptr;
		PulledComponent = nullptr;
		Pulling = false;

		// Try and play the sound if specified
		if (PullReleaseSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PullReleaseSound, GetOwner()->GetActorLocation());
		}
	}
	else
	{
		// Try and grab a physics object
		FHitResult Output;
		const FVector Start = Weapon_MuzzleLocation->GetComponentLocation();
		const FVector ForwardVector = Camera->GetForwardVector();
		const FVector End = ((ForwardVector * TargetRange) + Start);
		FCollisionQueryParams CollisionParams;
		if (GetWorld()->LineTraceSingleByChannel(Output, Start, End, ECC_Visibility, CollisionParams))
		{
			if (Output.bBlockingHit)
			{
				UPrimitiveComponent* OtherComp = Output.GetComponent();
				if ((OtherComp != NULL) && OtherComp->IsSimulatingPhysics() && OtherComp->GetMass() < MassPullLimit)
				{
					//Prepare to start pulling the object
					HitObject = true;
					Pulling = true;
					PulledComponent = OtherComp;
					PulledComponent->SetEnableGravity(false);
					PulledComponent->SetPhysicsLinearVelocity(FVector(0, 0, 0));
					StartPullPos = PulledComponent->GetComponentLocation();
					PulledActor = Output.GetActor();
					
					// Try and play the sound if specified
					if (PullSound != NULL)
					{
						UGameplayStatics::PlaySoundAtLocation(this, PullSound, GetOwner()->GetActorLocation());
					}
					
				}
			}
		}
	}
	return HitObject;
}
