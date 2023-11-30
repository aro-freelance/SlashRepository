// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Items/PotionTypes.h"
#include "Characters/SlashCharacter.h"
#include "RecoveryPickup.generated.h"




UCLASS()
class SLASH_API ARecoveryPickup : public AItem
{
	GENERATED_BODY()


protected:

	virtual void OnCollisionCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion Type")
	EPotionType PotionType = EPotionType::EPT_Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion Buff Type")
	EBuffType BuffType = EBuffType::EBT_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion Power")
	float PotionPower = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion Duration")
	float PotionDuration = 1.f;

private:


public:

	FORCEINLINE float GetPotionPower() const { return PotionPower; }
	FORCEINLINE float GetPotionDuration() const { return PotionDuration; }
	FORCEINLINE EPotionType GetPotionType() const { return PotionType; }
	FORCEINLINE EBuffType GetBuffType() const { return BuffType; }

};
