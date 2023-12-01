
#include "Items/RecoveryPickup.h"
#include "Interfaces/PickupInterface.h"

void ARecoveryPickup::OnCollisionCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnCollisionCapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	UE_LOG(LogTemp, Warning, TEXT("RecoveryPickup Collision Function."));

	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		bool ShouldDestroy = true;
		PickupSystem(ShouldDestroy);

		switch (PotionType)
		{
		case EPotionType::EPT_Health:
			PickupInterface->UseHealthPotion(this);
			break;
		case EPotionType::EPT_Mana:
			PickupInterface->UseManaPotion(this);
			break;
		case EPotionType::EPT_TP:
			PickupInterface->UseTPPotion(this);
			break;
		case EPotionType::EPT_Stamina:
			PickupInterface->UseStaminaPotion(this);
			break;
		case EPotionType::EPT_Buff:
			PickupInterface->UseBuffPotion(this);
			break;
		}

	}

	

}
