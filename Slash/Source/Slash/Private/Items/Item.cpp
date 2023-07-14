// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Slash/DebugMacros.h"



// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

	//float DeltaZ = Amplitude * FMath::Sin(RunningTime * Speed);

	//AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));

	//AddActorWorldRotation(FRotator(0.f, RotationRate, 0.f));

	UWorld* World = GetWorld();
	FVector Location = GetActorLocation();
	FRotator Rotation = GetActorRotation();
	FVector ForwardVector = GetActorForwardVector();
	float Length = 100.f;

	DRAW_SPHERE_SINGLE_FRAME(Location, FColor::Blue);
	DRAW_VECTOR_SINGLE_FRAME(Location, ForwardVector, Length, FColor::Green);


	
/*
	if (GEngine) {

		FString Name = GetName(); 

		int index = GetUniqueID();

		FString Message = FString::Printf(TEXT("Item name: %s"), *Name);

		GEngine->AddOnScreenDebugMessage(index, 10.f, FColor::Cyan, Message);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("GEngine is null."));
	} 
*/
	

}

