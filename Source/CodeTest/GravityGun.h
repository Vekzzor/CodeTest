// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "GravityGun.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CODETEST_API AGravityGun : public AWeapon
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AGravityGun();

	/** Sound to play each time we use primary fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound = nullptr;

	/** Sound to play each time we use secondary fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* PullSound = nullptr;

	/** Sound to play each time we use secondary fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* PullReleaseSound = nullptr;

	/** Fire rate for the weapon (in seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float FireRate;

	/** The force the weapon pushes away objects */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float PushMagnitude;

	/** The range which objects can be targeted */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float TargetRange;

	/** The speed which objects can be pulled */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float PullSpeedMultiplier;

	/** The max mass the weapon can pull */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float MassPullLimit;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when primary fire is triggered
	virtual bool FirePrimary(UCameraComponent* Camera) override;
	// Called when secondary fire is triggered
	virtual bool FireSecondary(UCameraComponent* Camera) override;
	// Called when the weapon is dropped
	virtual void Dropped(UCameraComponent* Camera) override;
	
	// Attempts to push away the targeted object and informs if successfull
	bool Push(UCameraComponent* Camera);
	// Attempts to pull the targeted object or drop it if already holding one
	bool Pull(UCameraComponent* Camera);


private:
	// Timer to track the fire rate 
	FTimerHandle FireRateTimer;

	// Alpha value for the linear interpolation during a pulling sequence
	float LerpAlpha = 0;

	// Holding an object or not
	bool Pulling = false;

	// Held collision component
	UPrimitiveComponent* PulledComponent = nullptr;

	// Held Actor object
	AActor* PulledActor = nullptr;

	// Start Position of the pulled object
	FVector StartPullPos;
};