// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "RecoveryPickup.generated.h"


//enum class EPotionType : uint8
//{
//	EPT_Health,
//	EPT_Mana,
//	EPT_TP,
//	EPT_Stamina,
//	EPT_Buff
//};
//
//enum class EBuffType : uint8
//{
//	EBT_None,
//	EBT_Invincible,
//	EBT_InfinMP,
//	EBT_InfinTP,
//	EBT_InfinStam,
//	EBT_XPMultiplier,
//	EBT_GoldMultiplier,
//	EBT_SpeedBoost,
//	EBT_PowerBoost
//	
//};

UCLASS()
class SLASH_API ARecoveryPickup : public AItem
{
	GENERATED_BODY()


protected:

	virtual void OnCollisionCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion Type")
	//EPotionType PotionType = EPotionType::EPT_Health;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion Buff Type")
	//EBuffType BuffType = EBuffType::EBT_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion Power")
	float PotionPower = 0.2f;

private:

	void HealthPotion();

	void ManaPotion();

	void TPPotion();

	void StaminaPotion();

	void BuffPotion();

};
