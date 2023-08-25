// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"


//this has been replaced by AttachMeshToSocket
/*
void AWeapon::Equip(USceneComponent* InParent, FName InSocketName)
{
	AttachMeshToSocket(InParent, InSocketName);
	ItemState = EItemState::EIS_Held;
}
*/


//constructor
AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());

	WeaponBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);

	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}


}

void AWeapon::DetachMeshFromSocket()
{
	FDetachmentTransformRules DeTransformRules(EDetachmentRule::KeepWorld, true);
	ItemMesh->DetachFromComponent(DeTransformRules);

	ItemMesh->SetWorldRotation(StartingRotation);

	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

}



void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

}

//perform a box trace to return information about collisions
void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(CanDoDamage())
	{

		
	
		const FVector Start = BoxTraceStart->GetComponentLocation();
		const FVector End = BoxTraceEnd->GetComponentLocation();
		//const FVector HalfSize = FVector(25.f); //5.f

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);

		for (AActor* Actor : HitActorsToIgnore)
		{
			ActorsToIgnore.Add(Actor);
		}

		FHitResult BoxHit;

		UKismetSystemLibrary::BoxTraceSingle(
			this,
			Start,
			End,
			HalfSize,
			BoxTraceStart->GetComponentRotation(),
			ETraceTypeQuery::TraceTypeQuery1,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			BoxHit,
			true
		);


	
		//if an actor was hit
		if (BoxHit.GetActor())
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *BoxHit.GetComponent()->GetName());
		
			IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
			if (HitInterface)
			{
				//call GetHit on the actor that was hit
				HitInterface->GetHit(BoxHit.ImpactPoint);
			}

			//ignore the hit actor so it cannot be hit multiple times by the same swing
			HitActorsToIgnore.AddUnique(BoxHit.GetActor());


			///TODO:  I am currently working on a bug involving Fields and Breakable objects. 
			//
			// SYMPTOM 1:
			// The object is breaking but the field is not the cause. 
			// It seems to be breaking based on impact with weapons. (still issue)
			// It was also breaking on impact with the character until I made it ignore pawns. 
			// 
			// SYMPTOM 2:
			// However, the fieldsystem object is having no effect. (Still issue)
			// 
			// SYMPTOM 3:
			// And the weapon doesn't seem to be firing a field system. (FIXED)
			// This is making it impossible to implement in a way that only fires on attack... (FIXED)
			// 
			// INFORMATION TO USE :
			// Relavent BP = BP_Weapon BP_WeaponLarge, BP_BreakablePot ... as well as SM Pot Geometry collection
			// Additionally, the BP_WeaponLarge does not seem to be causing it to break 
			// even though it has the same collision settings as BP_Weapon lookk in to this...
			//
			// PROGRESS:
			// added box collision to bp breakable pot 
			// this is now being reached... but the field is still not doing anything and the mesh of the sword is still breaking the pot. (but not the hammer).
			//
			//  TO fix this...
			//   1. make it so the mesh itself is not breaking the pot. can't solve the other issue until this is done.
			//        a. when looking in to this i might want to compare to the hammer (which is not breaking the pot)
			//   2. Once that is fixed I need to solve the field issue.
			//        a. the weapon field and the freestanding field are both not working to break the pot.
			//        b. try adjusting the parameters to extremes to see if it breaks
			//        c. rewatch 143 which is where we set up the fields
			//
			///
			CreateFields(BoxHit.ImpactPoint);

		}
	}	


}

bool AWeapon::CanDoDamage()
{
	//get the character that the weapon is attached to
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(GetAttachParentActor());
	if (!SlashCharacter)
	{
		return false;
	}
	else 
	{

		//if the character is attacking and the animation is at a point where damage is possible
		return SlashCharacter->GetActionState() == EActionState::EAS_Attacking &&
			GetWeaponCollisionState() == EWeaponCollisionState::EWS_CollisionOn;
	}
}

void AWeapon::PlayWeaponPickupSound()
{
	if (WeaponPickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation
		(
			this,
			WeaponPickupSound,
			GetActorLocation()
		);
	}
}
