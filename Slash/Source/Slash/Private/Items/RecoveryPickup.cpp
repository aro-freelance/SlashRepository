
#include "Items/RecoveryPickup.h"
#include "Interfaces/PickupInterface.h"

void ARecoveryPickup::OnCollisionCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnCollisionCapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	

	//switch (PotionType)
	//{
	//case EPotionType::EPT_Health:
	//	HealthPotion();
	//	break;
	//case EPotionType::EPT_Mana:
	//	ManaPotion();
	//	break;
	//case EPotionType::EPT_TP:
	//	TPPotion();
	//	break;
	//case EPotionType::EPT_Stamina:
	//	StaminaPotion();
	//	break;
	//case EPotionType::EPT_Buff:
	//	BuffPotion();
	//	break;
	//}

}

void ARecoveryPickup::HealthPotion()
{
	//TODO: heal for value
}

void ARecoveryPickup::ManaPotion()
{
	//TODO: mp heal for value
}

void ARecoveryPickup::TPPotion()
{
	//TODO: tp heal for value
}

void ARecoveryPickup::StaminaPotion()
{
	//TODO: stam heal for value
}

void ARecoveryPickup::BuffPotion()
{
	//TODO: switch based on buff type and execute function for that buff (make new functions)
}
