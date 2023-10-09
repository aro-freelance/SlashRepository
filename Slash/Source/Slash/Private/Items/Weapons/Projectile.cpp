// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Projectile.h"
#include "Items/Weapons/Weapon.h"
#include "Interfaces/HitInterface.h"

// Sets default values
AProjectile::AProjectile()
{
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;	

	ItemMesh->BodyInstance.SetCollisionProfileName("Projectile");
	ItemMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	ItemMesh->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	ItemMesh->CanCharacterStepUpOn = ECB_No;
	
}

void AProjectile::Tick(float DeltaTime)
{
	if (!CharacterWhoFiredThis || !WeaponThatFiredThis)
	{
		InitializeShooterInfo();
	}

	//if it hasn't been added yet, but we have the character and weapon, then add an impulse to the projectile
	if (!HasInitialImpulse && CharacterWhoFiredThis && WeaponThatFiredThis)
	{
		UE_LOG(LogTemp, Warning, TEXT("C++ add initial impulse"));
		HasInitialImpulse = true;

		

		FVector ShooterForwardVector = CharacterWhoFiredThis->GetActorForwardVector();
		float Speed = WeaponThatFiredThis->GetProjectileSpeedMultiplier();
		//FVector Offset = (ShooterForwardVector * OffsetAmount);

		////set the projectile in front of the player first
		//this->SetActorLocation(CharacterWhoFiredThis->GetActorLocation() + Offset);

		//shoot the projectile forward in the direction the character is facing
		ItemMesh->AddImpulse(FVector(ShooterForwardVector * Speed));

	}

}


void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Projectile. BeginPlayC++."));


}

void AProjectile::InitializeShooterInfo()
{
	AActor* ProjectileOwner = this->GetOwner();

	if (ProjectileOwner)
	{
		FString OwnerName = ProjectileOwner->GetName();
		UE_LOG(LogTemp, Warning, TEXT("weapon projectile spawned. ownername = %s"), *OwnerName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ProjectileOwner null"));
	}

	WeaponThatFiredThis = Cast<AWeapon>(this->GetOwner());
	if (WeaponThatFiredThis != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("initialize shooter on projectile: weapon cast succeed"));
		CharacterWhoFiredThis = Cast<ASlashCharacter>(WeaponThatFiredThis->GetAttachParentActor());
		if (CharacterWhoFiredThis != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("initialize shooter on projectile: slash character cast succeed"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("initialize shooter on projectile: slash character cast failed"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("initialize shooter on projectile: weapon cast failed."));
	}
}



//TODO: this is not currently detecting hits. Fix this.
void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	

	if (OtherActor == nullptr) 
	{
		UE_LOG(LogTemp, Warning, TEXT("other actor null"));
		return;
	}

	if (OtherActor == this)
	{
		UE_LOG(LogTemp, Warning, TEXT("other actor is the shooter. hit yourself"));
		return;
	}

	if (OtherComp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("other component is null"));
		return;
	}

	//if (!OtherComp->IsSimulatingPhysics())
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("other component is not simulating physics"));
	//	return;
	//}

	////add physics to hit actor
	//OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

	if (!CharacterWhoFiredThis || !WeaponThatFiredThis)
	{
		InitializeShooterInfo();

		if (!CharacterWhoFiredThis || !WeaponThatFiredThis)
		{
			return;
		}
	}

	if (CharacterWhoFiredThis && WeaponThatFiredThis) 
	{
		/*if (WeaponThatFiredThis->CanDoDamage())
		{

			UE_LOG(LogTemp, Warning, TEXT("Onhit. can do damage."));
			

		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("weapon fired cannot do damage"));
		}*/


		//get the hit interface of the actor who was hit
		IHitInterface* HitInterface = Cast<IHitInterface>(OtherActor);

		if (HitInterface)
		{

			UE_LOG(LogTemp, Warning, TEXT("calling get hit."));
			//call GetHit on the actor that was hit
			HitInterface->Execute_GetHit(OtherActor, Hit.ImpactPoint, CharacterWhoFiredThis, WeaponThatFiredThis);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("projectile call to hitinterface failed"));
		}

		//ignore the hit actor so it cannot be hit multiple times by the same swing
		WeaponThatFiredThis->HitActorsToIgnore.AddUnique(OtherActor);


		Destroy();
	}
	else
	{
		//TODO: this is currently the issue.  keep working on this from here. Weapon.cpp 70. 
		UE_LOG(LogTemp, Warning, TEXT("failed to initialize shooter info"));
	}


}

//	// Use a sphere as a simple collision representation
//	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
//	CollisionComp->InitSphereRadius(5.0f);
//	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
//	CollisionComp->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);		// set up a notification for when this component hits something blocking
//
//	// Players can't walk on it
//	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
//	CollisionComp->CanCharacterStepUpOn = ECB_No;
//
//	// Set as root component
//	RootComponent = CollisionComp;
//
//	// Use a ProjectileMovementComponent to govern this projectile's movement
//	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
//	ProjectileMovement->UpdatedComponent = CollisionComp;
//	ProjectileMovement->InitialSpeed = 3000.f;
//	ProjectileMovement->MaxSpeed = 3000.f;
//	ProjectileMovement->bRotationFollowsVelocity = true;
//	ProjectileMovement->bShouldBounce = true;
//
//	// Die after 3 seconds by default
//	InitialLifeSpan = 3.0f;
//}
//
//void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
//{
//	// Only add impulse and destroy projectile if we hit a physics
//	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
//	{
//		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
//
//		Destroy();
//	}
//}