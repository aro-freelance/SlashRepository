#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickupInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SLASH_API IPickupInterface
{
	GENERATED_BODY()

public:

	virtual void SetOverlappingItem(class AItem* Item);
	virtual void AddSouls(class ASoul* Soul);
	virtual void UseHealthPotion(class ARecoveryPickup* Potion);
	virtual void UseManaPotion(class ARecoveryPickup* Potion);
	virtual void UseTPPotion(class ARecoveryPickup* Potion);
	virtual void UseStaminaPotion(class ARecoveryPickup* Potion);
	virtual void UseBuffPotion(class ARecoveryPickup* Potion);

};
