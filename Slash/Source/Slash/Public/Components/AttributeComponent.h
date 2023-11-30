
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
	float Stamina = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Stats")
	float MaxHP = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Stats")
	float MaxMP = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Stats")
	float MaxTP = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Stats")
	float MaxStamina = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	float XP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	float CurrentLevelXpToNextLevel = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	float Level = 1;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gold")
	float Gold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Souls")
	float Souls;


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
	float RegenPercent = 0.055; //TODO: .025

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	float StaminaRegenPercent = 0.09; //TODO: .025

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Drops)
	int32 SoulDropAmount = 1;

private:	
	
	
	
public:

	void ReceiveDamage(float Damage);
	void UseStamina(float StaminaCost);

	float GetHealthPercent();
	float GetTPPercent();
	float GetStaminaPercent();

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
	float GetStamina();
	float GetMaxStamina();
	
	float GetRegenPercent();
	float GetStaminaRegenPercent();
		
	void SetHP(float NewHP) { HP = NewHP; }

	void SetTP(float NewTP) { TP = NewTP; }

	void SetStamina(float NewStamina) { Stamina = NewStamina; }

	void SetGold(int32 NewGold) { Gold = NewGold; }
	void SetSouls(int32 NewSouls) { Souls = NewSouls; }

	int32 GetGold() const { return Gold; }
	int32 GetSouls() const { return Souls; }

	int32 GetSoulDropAmount() const { return SoulDropAmount; }

	bool IsAlive();

};
