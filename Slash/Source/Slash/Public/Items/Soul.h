#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"



UCLASS()
class SLASH_API ASoul : public AItem
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Value)
	int32 Value = 1;

	virtual void OnCollisionCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:


public:

	FORCEINLINE int32 GetValue() const { return Value; }
	FORCEINLINE void SetValue(int32 NewValue) { Value = NewValue; }
	
};
