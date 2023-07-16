// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Slash/DebugMacros.h"



// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;

}

void AItem::BeginPlay()
{
	Super::BeginPlay();

}

float AItem::TransformedSin()
{
	return SinAmplitude * FMath::Sin(RunningTime * SinSpeed);
}

float AItem::TransformedCos()
{
	return CosAmplitude * FMath::Cos(RunningTime * CosSpeed);
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	AddActorWorldRotation(FRotator(
		(RotationRateX * DeltaTime), (RotationRateY * DeltaTime), (RotationRateZ * DeltaTime)));

}

