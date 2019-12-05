// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
// Sets default values
AWeapon::AWeapon()
{
	// Create a box physics component
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	CollisionComp->InitBoxExtent(FVector(3.905719f, 39.580437f, 15.95967f));
	CollisionComp->BodyInstance.SetCollisionProfileName("PhysicsActor");
	CollisionComp->SetSimulatePhysics(true);

	// Create a weapon mesh component
	Weapon_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	Weapon_Mesh->SetupAttachment(CollisionComp);
	Weapon_Mesh->RelativeLocation = FVector(0.0f, -14.0f, -3.0f);
	Weapon_Mesh->SetOnlyOwnerSee(false);			// only the owning player will see this mesh
	Weapon_Mesh->bCastDynamicShadow = false;
	Weapon_Mesh->CastShadow = false;

	// Create a muzzle location component
	Weapon_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	Weapon_MuzzleLocation->SetupAttachment(Weapon_Mesh);
	Weapon_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
	Weapon_MuzzleLocation->bVisualizeComponent = true;

	// Default offset from the muzzle of the weapon
	MuzzleOffset = FVector(200.0f, 0.0f, 10.0f);

	// Set this component to be initialized when the game starts, and to be ticked every frame
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called when the weapon is dropped
void AWeapon::Dropped(UCameraComponent * Camera)
{
	// Throw the object slightly in the aimed direction
	FVector Velocity = GetOwner()->GetVelocity();
	DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	SetOwner(this);

	CollisionComp->BodyInstance.SetCollisionProfileName("PhysicsActor");
	CollisionComp->SetSimulatePhysics(true);
	CollisionComp->AddWorldTransform(FTransform(FVector(Camera->GetForwardVector() * 120.f)));
	CollisionComp->AddImpulseAtLocation(Velocity + Camera->GetForwardVector()*1000.0f, GetActorLocation());
}