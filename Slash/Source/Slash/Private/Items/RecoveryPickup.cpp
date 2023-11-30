
#include "Items/RecoveryPickup.h"
#include "Interfaces/PickupInterface.h"

void ARecoveryPickup::OnCollisionCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnCollisionCapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);

	if (SlashCharacter)
	{

		switch (PotionType)
		{
		case EPotionType::EPT_Health:
			SlashCharacter->UseHealthPotion(this);
			break;
		case EPotionType::EPT_Mana:
			SlashCharacter->UseManaPotion(this);
			break;
		case EPotionType::EPT_TP:
			SlashCharacter->UseTPPotion(this);
			break;
		case EPotionType::EPT_Stamina:
			SlashCharacter->UseStaminaPotion(this);
			break;
		case EPotionType::EPT_Buff:
			SlashCharacter->UseBuffPotion(this);
			break;
		}
	}

}
