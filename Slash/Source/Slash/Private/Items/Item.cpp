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

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//movement in cm/frame = (cm/s) * (s/frame)
	float MovementRate = 50.f * DeltaTime;

	float RotationRate = 45.f * DeltaTime;

	AddActorWorldOffset(FVector(MovementRate, 0.f, 0.f));

	AddActorWorldRotation(FRotator(0.f, RotationRate, 0.f));

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

