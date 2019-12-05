// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Weapon.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CODETEST_API AWeapon : public AActor
{
	GENERATED_BODY()
	
	

public:	
	// Sets default values for this actor's properties
	AWeapon();

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Weapon_Mesh = nullptr;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USceneComponent* Weapon_MuzzleLocation = nullptr;

	/** Box collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Collision)
		class UBoxComponent* CollisionComp;
public:	

	// Called when primary fire is triggered
	virtual bool FirePrimary(UCameraComponent* Camera) { return false; };
	// Called when secondary fire is triggered
	virtual bool FireSecondary(UCameraComponent* Camera) { return false; };

	// Called when the weapon is dropped
	virtual void Dropped(UCameraComponent* Camera);

};
