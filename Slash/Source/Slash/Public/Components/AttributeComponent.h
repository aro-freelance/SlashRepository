
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


protected:
	virtual void BeginPlay() override;

private:	
	
	UPROPERTY(EditAnywhere, Category = "Dynamic Stats")
	float HP = 100;
	UPROPERTY(EditAnywhere, Category = "Dynamic Stats")
	float MP = 100;
	UPROPERTY(EditAnywhere, Category = "Dynamic Stats")
	float TP = 0;
	UPROPERTY(EditAnywhere, Category = "Dynamic Stats")
	float MaxHP = 100;
	UPROPERTY(EditAnywhere, Category = "Dynamic Stats")
	float MaxMP = 100;
	UPROPERTY(EditAnywhere, Category = "Dynamic Stats")
	float MaxTP = 100;

	UPROPERTY(EditAnywhere, Category = "Experience")
	float XP;
	UPROPERTY(EditAnywhere, Category = "Experience")
	float CurrentLevelXpToNextLevel = 10;
	UPROPERTY(EditAnywhere, Category = "Experience")
	float Level = 1;


	UPROPERTY(EditAnywhere, Category = "Gold")
	float Gold;
	

	UPROPERTY(EditAnywhere, Category = "Character Stats")
	float STR = 1;
	UPROPERTY(EditAnywhere, Category = "Character Stats")
	float VIT = 1;
	UPROPERTY(EditAnywhere, Category = "Character Stats")
	float DEX = 1;
	UPROPERTY(EditAnywhere, Category = "Character Stats")
	float AGI = 1;
	UPROPERTY(EditAnywhere, Category = "Character Stats")
	float MND = 1;
	UPROPERTY(EditAnywhere, Category = "Character Stats")
	float CHR = 1;
	
public:

	void ReceiveDamage(float Damage);
	float GetHealthPercent();

	float GetStr();
	float GetVit();
	float GetDex();
	float GetAgi();
		
};
