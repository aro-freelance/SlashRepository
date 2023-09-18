

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

float UAttributeComponent::GetHealthPercent()
{
	return (HP / MaxHP);
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


void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

