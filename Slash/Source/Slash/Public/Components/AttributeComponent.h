
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

//

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Stats")
	float HP = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Stats")
	float MP = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Stats")
	float TP = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Stats")
	float MaxHP = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Stats")
	float MaxMP = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Stats")
	float MaxTP = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	float XP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	float CurrentLevelXpToNextLevel = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	float Level = 1;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gold")
	float Gold;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float STR = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float VIT = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float DEX = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float AGI = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float INT = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float MND = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	float CHR = 1;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	float RegenPercent;
	
	

private:	
	
	
	
public:

	void ReceiveDamage(float Damage);
	float GetHealthPercent();

	float GetStr();
	float GetVit();
	float GetDex();
	float GetAgi();
	float GetInt();
	float GetMnd();
	float GetChr();
	
	float GetHP();
	float GetMaxHP();
	float GetMP();
	float GetMaxMP();
	float GetTP();
	float GetMaxTP();
		
	void SetHP(float NewHP) { HP = NewHP; }

	bool IsAlive();

};
