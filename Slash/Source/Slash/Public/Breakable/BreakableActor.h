// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;

UCLASS()
class SLASH_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();

	virtual void Tick(float DeltaTime) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, ACharacter* DamageDealer, AWeapon* Weapon);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry Collection")
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Treasure")
	bool ShouldSpawnTreasureOnBreak = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Potions and Buffs")
	bool ShouldSpawnRecoveryPickupOnBreak = false;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Treasure")
	//TSubclassOf<class ATreasure> TreasureToSpawnOnBreak;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Treasure")
	TArray<TSubclassOf<class ATreasure>> ArrayOfTreasureToSpawnOnBreak;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Potions and Buffs")
	TArray<TSubclassOf<class ARecoveryPickup>> ArrayOfPotionsNBuffsToSpawnOnBreak;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool HasBeenHit = false;


private:	
	
	
};
