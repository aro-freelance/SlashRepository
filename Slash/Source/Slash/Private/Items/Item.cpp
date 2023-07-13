// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "DrawDebugHelpers.h"
#include "Slash/Slash.h"



// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AItem::BeginPlay()
{
	Super::BeginPlay();


	
	UWorld* World = GetWorld();

	FVector Location = GetActorLocation();

	FVector ForwardVector = GetActorForwardVector();
	
	/*
	if (World) {
		DrawDebugLine(World, Location, Location + (ForwardVector * 100), FColor::Red, true);
	}*/


	DebugLine(Location, ForwardVector);
	DrawSphere(Location);
	
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

	if (GEngine) {

		FString Name = GetName(); 

		int index = GetUniqueID();

		FString Message = FString::Printf(TEXT("Item name: %s"), *Name);

		GEngine->AddOnScreenDebugMessage(index, 10.f, FColor::Cyan, Message);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("GEngine is null."));
	}
	
}

