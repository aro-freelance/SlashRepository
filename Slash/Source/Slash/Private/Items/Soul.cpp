// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"
#include "Interfaces/PickupInterface.h"
#include "Characters/SlashCharacter.h"


void ASoul::OnCollisionCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Super::OnCollisionCapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);

	if (PickupInterface)
	{
		ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
		if (SlashCharacter)
		{
			SlashCharacter->AbortAttack();
		}

		PickupInterface->AddSouls(this);

		bool ShouldDestroy = true;
		PickupSystem(ShouldDestroy);
	}
	
	

}

