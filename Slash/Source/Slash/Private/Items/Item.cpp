// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Slash/DebugMacros.h"
#include "Components/SphereComponent.h"
#include "Characters/SlashCharacter.h"


AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());

}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

}

float AItem::TransformedSin()
{
	return SinAmplitude * FMath::Sin(RunningTime * SinSpeed);
}

float AItem::TransformedCos()
{
	return CosAmplitude * FMath::Cos(RunningTime * CosSpeed);
}


void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);

	if (SlashCharacter) 
	{
		SlashCharacter->SetOverlappingItem(this);
	}

}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);

	if (SlashCharacter)
	{
		SlashCharacter->SetOverlappingItem(nullptr);
	}

}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}

	if (ItemState == EItemState::EIS_Rotating)
	{
		AddActorWorldRotation(FRotator(
			(RotationRateX * DeltaTime), (RotationRateY * DeltaTime), (RotationRateZ * DeltaTime)));
	}
	

}

