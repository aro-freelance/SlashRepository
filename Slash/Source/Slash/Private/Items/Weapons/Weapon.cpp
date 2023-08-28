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

		UE_LOG(LogTemp, Warning, TEXT("Can do damage. Overlapped with Actor: %s . Component: %s"), 
			*OtherActor->GetName(), *OtherComp->GetName());


		FHitResult BoxHit;
		BoxTrace(BoxHit);

		FString BoxHitString = BoxHit.ToString();

		//DEBUG INFO: When this is used here it does not destroy the pot. Tested here to see about not needing actor hit. 
		//CreateFields(BoxHit.ImpactPoint);
	
		//if an actor was hit
		if (BoxHit.GetActor())
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit! Actor: %s. Component: %s."), 
				*BoxHit.GetActor()->GetName(), *BoxHit.GetComponent()->GetName());
	
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
			// It seems to be breaking based on impact with weapons. (FIXED)
			// It was also breaking on impact with the character until I made it ignore pawns. 
			// 
			// SYMPTOM 2:
			// However, the fieldsystem object is having no effect. (FIXED)
			// 
			// SYMPTOM 3:
			// And the weapon doesn't seem to be firing a field system. (FIXED)
			// This is making it impossible to implement in a way that only fires on attack... (FIXED)
			// 
			// SYMPTON 4:
			// The field is only firing when hitting an actor. Which it is not doing with the breakable pot geometry.
			// It is firing when hitting the enemy mesh. See the following debug messages:
			// 
			// Attack on Enemy
			// LogTemp: Warning: Can attack
			// LogTemp: Warning: Can do damage.Overlapped with Actor : BP_Enemy_C_1.Component : CharacterMesh0
			// LogTemp : Warning: Hit!Actor : BP_Enemy_C_1.Component : CharacterMesh0.
			// LogTemp: Warning: BoxHit Output : bBlockingHit:True bStartPenetrating : False Time : 0.020287 Location : X = 1304.702 Y = -230.651 Z = 134.774 ImpactPoint : X = 1312.262 Y = -233.074 Z = 131.315 Normal : X = -0.897 Y = 0.389 Z = -0.209 ImpactNormal : X = -0.897 Y = 0.389 Z = -0.209 TraceStart : X = 1302.932 Y = -231.766 Z = 134.712 TraceEnd : X = 1390.192 Y = -176.806 Z = 137.805 PenetrationDepth : 0.000000 Item : 0 PhysMaterial : DefaultPhysicalMaterial Actor : BP_Enemy_C_1 Component : CharacterMesh0 BoneName : Hips FaceIndex : -1
			// 
			// Attack on Pot
			// LogTemp: Warning: Can attack
			// LogTemp: Warning: Can do damage.Overlapped with Actor : BP_BreakablePot_C_1.Component : Capsule
			// LogTemp : Warning: Did not hit actor.
			// LogTemp : Warning : BoxHit Output : bBlockingHit:False bStartPenetrating : False Time : 1.000000 Location : X = 0.000 Y = 0.000 Z = 0.000 ImpactPoint : X = 0.000 Y = 0.000 Z = 0.000 Normal : X = 0.000 Y = 0.000 Z = 0.000 ImpactNormal : X = 0.000 Y = 0.000 Z = 0.000 TraceStart : X = 1230.272 Y = 1.616 Z = 165.484 TraceEnd : X = 1195.764 Y = 23.344 Z = 260.256 PenetrationDepth : 0.000000 Item : 0 PhysMaterial : None Actor : None Component : None BoneName : None FaceIndex : 0
			//
			// Important: Enemy is being hit in CharacterMesh.
			// 
			// Note: The field system is working just not firing as proved by putting a pot behind and enemy. 
			// Attacking an enemy fires the field system and breaks the pot behind the enemy. This is not desired, 
			// and should be fixed by making the field system smaller or making it fire from the hit actor instead,
			// and calling it from the pot. ... can only do this once it is registering hit on pot.
			// 
			// INFORMATION TO USE :
			// Relavent BP = BP_Weapon BP_WeaponLarge, BP_BreakablePot ... as well as SM Pot Geometry collection
			// Additionally, the BP_WeaponLarge does not seem to be causing it to break 
			// even though it has the same collision settings as BP_Weapon lookk in to this...
			//
			// PROGRESS:
			// (3... but this is likely still an issue. see symptom 4)
			// added box collision to bp breakable pot 
			// this is now being reached... but the field is still not doing anything and the mesh of the sword is still breaking the pot. (but not the hammer).
			//
			// (1)
			// The sword had a simple collision capsule and the hammer did not. This was causing the unintended breaking. 
			// Removed collision capsule for now. Might be better to find a way to keep it but make it not destroy pot.
			// But for now not having it and then getting the field to work would be best.
			// 
			// (2)
			// The standalone field system is working again now. 
			// It needed to be given a delay between the radial force and the linear force. 
			// Also the linear force needed a dynamic state type meta data filter.
			// 
			// Added the working field system to sword. only firing when boxhit is hitting an actor.
			// 
			// (4)
			// The pot geometry collection component (the fracture mesh) needed Generate Overlap Events checked.
			// It is now checked like it was on the mesh for the enemy and is now working. 
			// Will need to adjust the strength of the field but it is now working.
			// 
			// 
			// 
			//  TO fix this...
			//   1. make it so the mesh itself is not breaking the pot. can't solve the other issue until this is done. (done)
			//        a. when looking in to this i might want to compare to the hammer (which is not breaking the pot) 
			//   2. Once that is fixed I need to solve the field issue. (done. Field is working)
			//        a. the weapon field and the freestanding field are both not working to break the pot. 
			//        b. try adjusting the parameters to extremes to see if it breaks
			//        c. rewatch 143 which is where we set up the fields
			//  3. Make hits register with pot. (done. Generate Overlap Events needed to be checked on the pot)
			//      a. currently we are getting overlaps but no hit on actor most of the time. 
			//      b. capsule was added but is likely part of the issue.
			//      c. collision with the actual breakable geometry needs to happen.
			// 
			// NEXT: Adjust the Damage thresholds on the pot and the field magnitudes for the sword until the 
			// breaking affect looks better. 
			// And then update hammer to match sword or possibly to be stronger than sword at crushing pots.
			// 
			
			
			CreateFields(BoxHit.ImpactPoint);

		}
		else 
		{
			UE_LOG(LogTemp, Warning, TEXT("Did not hit actor."));
		}

		UE_LOG(LogTemp, Warning, TEXT("BoxHit Output: %s"), *BoxHitString);
	}	
	else
	{
		
		FString StateString = BuildStateString();

		UE_LOG(LogTemp, Warning, TEXT("Can't do damage: %s."), *StateString);
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

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();
	//const FVector HalfSize = FVector(25.f); //5.f

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

	for (AActor* Actor : HitActorsToIgnore)
	{
		ActorsToIgnore.Add(Actor);
	}



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

FString AWeapon::BuildStateString()
{
	FString StateString = FString();
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(GetAttachParentActor());

	if (!SlashCharacter) 
	{
		StateString = "Slash Character does not exist";
		return StateString; 
	}

	switch (SlashCharacter->GetActionState())
	{
	case EActionState::EAS_Unoccupied:
		StateString = "Unoccupied.";
		break;
	case EActionState::EAS_Equipping:
		StateString = "Equipping.";
		break;
	case EActionState::EAS_Attacking:
		StateString = "Attacking.";
		break;
	default:
		break;
	}

	switch (SlashCharacter->GetCharacterState())
	{
	case ECharacterState::ECS_Unarmed:
		StateString = StateString + " Unarmed";
		break;
	case ECharacterState::ECS_EquippedOneHanded:
		StateString = StateString + " Equipped One Handed";
		break;
	case ECharacterState::ECS_EquippedTwoHanded:
		StateString = StateString + " Equipped Two Handed";
		break;
	default:
		break;
	}

	switch (GetWeaponCollisionState())
	{
	case EWeaponCollisionState::EWS_CollisionOn:
		StateString = StateString + " Collision On";
		break;
	case EWeaponCollisionState::EWS_CollisionOff:
		StateString = StateString + " Collision Off";
		break;
	default:
		break;
	}

	return StateString;
}
