// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Characters/SlashCharacter.h"
#include "HitInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SLASH_API IHitInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//this allows it to be called in c++ as well as blueprints
	UFUNCTION(BlueprintNativeEvent)
	void GetHit(const FVector& ImpactPoint, ASlashCharacter* DamagerDealer, float Damage, int32 PrecisionRange, int32 LowAccFloor, int32 HighAccFloor, float PercentMagicDamage, const FString& WeaponName);

};
