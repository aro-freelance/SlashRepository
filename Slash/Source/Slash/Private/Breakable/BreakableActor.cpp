// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasure.h"
#include "Items/RecoveryPickup.h"
#include "Components/CapsuleComponent.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("Geometry Collection"));
	SetRootComponent(GeometryCollection);

	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	//GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Ignore);


	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}



void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, ACharacter* DamageDealer, AWeapon* Weapon)
{

	//additional functionality in Blueprints

	if (ShouldSpawnTreasureOnBreak) 
	{
		UWorld* World = GetWorld();
		if (World && ArrayOfTreasureToSpawnOnBreak.Num() > 0)
		{
			const int32 RandomSelection = FMath::RandRange(0, ArrayOfTreasureToSpawnOnBreak.Num() - 1);

			World->SpawnActor<ATreasure>
			(
				ArrayOfTreasureToSpawnOnBreak[RandomSelection], 
				GetActorLocation(), 
				GetActorRotation()
			);
		}
	}

	if (ShouldSpawnRecoveryPickupOnBreak)
	{
		UWorld* World = GetWorld();
		if (World && ArrayOfPotionsNBuffsToSpawnOnBreak.Num() > 0)
		{
			const int32 RandomSelection = FMath::RandRange(0, ArrayOfPotionsNBuffsToSpawnOnBreak.Num() - 1);

			World->SpawnActor<ARecoveryPickup>
				(
					ArrayOfPotionsNBuffsToSpawnOnBreak[RandomSelection],
					GetActorLocation(),
					GetActorRotation()
				);
		}

	}
	

}

