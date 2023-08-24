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

		
		FString Result = BoxHit.ToString();
	
		//if an actor was hit
		if (BoxHit.GetActor())
		{

			UE_LOG(LogTemp, Warning, TEXT("an actor was hit. BoxHit Result = %s"), *Result);

			//BoxHit Result = 
			// bBlockingHit:True 
			// bStartPenetrating:False 
			// Time:0.595645 
			// Location:X=1325.158 Y=-276.383 Z=159.433 
			// ImpactPoint:X=1328.139 Y=-268.866 Z=162.533 
			// Normal:X=-0.492 Y=-0.822 Z=-0.288 
			// ImpactNormal:X=-0.492 Y=-0.822 Z=-0.288 
			// TraceStart:X=1266.720 Y=-287.827 Z=144.247 
			// TraceEnd:X=1364.829 Y=-268.614 Z=169.743 
			// PenetrationDepth:0.000000 
			// Item:2 
			// PhysMaterial:DefaultPhysicalMaterial 
			// Actor:BP_Enemy_C_1 
			// Component:CharacterMesh0 
			// BoneName:Spine1 
			// FaceIndex:-1


			//BoxHit Result = 
			// bBlockingHit:True 
			// bStartPenetrating : False 
			// Time : 0.253133 
			// Location : X = 1485.689 Y = 1390.236 Z = 118.256 
			// ImpactPoint : X = 1459.382 Y = 1447.237 Z = 138.700 
			// Normal : X = -0.098 Y = -0.977 Z = 0.191 
			// ImpactNormal : X = -0.098 Y = -0.977 Z = 0.191 
			// TraceStart : X = 1475.293 Y = 1361.287 Z = 123.148 
			// TraceEnd : X = 1516.364 Y = 1475.647 Z = 103.822 
			// PenetrationDepth : 0.000000 
			// Item : 0 
			// PhysMaterial : DefaultPhysicalMaterial 
			// Actor : BP_Enemy_C_3 
			// Component : CharacterMesh0 
			// BoneName : Hips 
			// FaceIndex : -1



			//if that actor has the hit interface
			IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
			if (HitInterface)
			{
				//call GetHit on the actor that was hit
				HitInterface->GetHit(BoxHit.ImpactPoint);
			}

			//ignore the hit actor so it cannot be hit multiple times by the same swing
			HitActorsToIgnore.AddUnique(BoxHit.GetActor());
		}
		else
		{
			
			UE_LOG(LogTemp, Warning, TEXT("trace happened. no hit. BoxHit Result = %s"), *Result);

			//BoxHit Result = 
			// bBlockingHit:False 
			// bStartPenetrating:False 
			// Time:1.000000 
			// Location:X=0.000 Y=0.000 Z=0.000 
			// ImpactPoint:X=0.000 Y=0.000 Z=0.000 
			// Normal:X=0.000 Y=0.000 Z=0.000 
			// ImpactNormal:X=0.000 Y=0.000 Z=0.000 
			// TraceStart:X=1374.295 Y=397.920 Z=124.403 
			// TraceEnd:X=1490.013 Y=434.798 Z=104.951 
			// PenetrationDepth:0.000000 
			// Item:0
			// PhysMaterial:None 
			// Actor:None
			// Component:None 
			// BoneName:None 
			// FaceIndex:0


		}
	}	
	else
	{
		if (CanAttack) 
		{
			UE_LOG(LogTemp, Warning, TEXT("cannot do damage but can attack"));
		}
		else 
		{
			UE_LOG(LogTemp, Warning, TEXT("cannot attack"));
		}
		if (CanCollide)
		{
			UE_LOG(LogTemp, Warning, TEXT("cannot do damage but can collide"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("cannot collide"));
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
		//This is for debugging///////////////
		
		if (SlashCharacter->GetActionState() != EActionState::EAS_Attacking)
		{
			CanAttack = false;
			
		}
		else
		{
			CanAttack = true;
		}
		if (GetWeaponCollisionState() != EWeaponCollisionState::EWS_CollisionOn)
		{
			CanCollide = false;
			
		}
		else
		{
			CanCollide = true;
		}
		////////////////////////////////////////////////

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
