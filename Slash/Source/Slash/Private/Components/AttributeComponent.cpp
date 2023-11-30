

#include "Components/AttributeComponent.h"


UAttributeComponent::UAttributeComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UAttributeComponent::ReceiveDamage(float Damage)
{
	//Damage is modified in cpp for the actor taking damage. i.e. Enemy.cpp TakeDamage

	HP = FMath::Clamp(HP - Damage, 0.f, MaxHP);

}

void UAttributeComponent::UseStamina(float StaminaCost)
{
	Stamina = FMath::Clamp(Stamina - StaminaCost, 0.f, MaxStamina);
}

float UAttributeComponent::GetHealthPercent()
{
	return (HP / MaxHP);
}

float UAttributeComponent::GetTPPercent()
{
	return (TP / MaxTP);
}

float UAttributeComponent::GetStaminaPercent()
{
	return (Stamina / MaxStamina);
}

float UAttributeComponent::GetStr()
{
	return STR;
}

float UAttributeComponent::GetVit()
{
	return VIT;
}

float UAttributeComponent::GetDex()
{
	return DEX;
}

float UAttributeComponent::GetAgi()
{
	return AGI;
}

float UAttributeComponent::GetInt()
{
	return INT;
}

float UAttributeComponent::GetMnd()
{
	return MND;
}

float UAttributeComponent::GetChr()
{
	return CHR;
}

float UAttributeComponent::GetHP()
{
	return HP;
}

float UAttributeComponent::GetMaxHP()
{
	return MaxHP;
}
float UAttributeComponent::GetMP()
{
	return MP;
}

float UAttributeComponent::GetMaxMP()
{
	return MaxMP;
}

float UAttributeComponent::GetTP()
{
	return TP;
}

float UAttributeComponent::GetMaxTP()
{
	return MaxTP;
}

float UAttributeComponent::GetStamina()
{
	return Stamina;
}

float UAttributeComponent::GetMaxStamina()
{
	return MaxStamina;
}

float UAttributeComponent::GetRegenPercent()
{
	return RegenPercent;
}

float UAttributeComponent::GetStaminaRegenPercent()
{
	return StaminaRegenPercent;
}

bool UAttributeComponent::IsAlive()
{
	bool HasHP = true;
	if (HP <= 0) { HasHP = false; }

	return HasHP;
}


void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

