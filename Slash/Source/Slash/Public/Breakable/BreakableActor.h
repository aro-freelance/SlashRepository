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

	virtual void GetHit_Implementation(const FVector& ImpactPoint, ASlashCharacter* DamageDealer, float Damage = 20.f, int32 PrecisionRange = 15, int32 LowAccFloor = 90, int32 HighAccFloor = 95, float PercentMagicDamage = 0.0f, const FString& WeaponName = "default") override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry Collection")
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Treasure")
	bool ShouldSpawnTreasureOnBreak;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Treasure")
	//TSubclassOf<class ATreasure> TreasureToSpawnOnBreak;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Treasure")
	TArray<TSubclassOf<class ATreasure>> ArrayOfTreasureToSpawnOnBreak;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool HasBeenHit = false;


private:	
	
	
};
