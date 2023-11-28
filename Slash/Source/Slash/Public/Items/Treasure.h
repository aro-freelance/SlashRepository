// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Treasure.generated.h"

UCLASS()
class SLASH_API ATreasure : public AItem
{
	GENERATED_BODY()

protected:

	virtual void OnCollisionCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:


	UPROPERTY(EditAnywhere, Category = "Treasure Properties")
	int32 Gold;

	//UPROPERTY(EditAnywhere, Category = "Treasure Properties")
	//int32 Souls;

//Getters and Setters
public:
	FORCEINLINE int32 GetGoldAmount() const { return Gold; }

	//FORCEINLINE int32 GetSoulsAmount() const { return Souls; }

};
