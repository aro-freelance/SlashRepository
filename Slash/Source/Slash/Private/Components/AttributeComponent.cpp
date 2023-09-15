

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
	//Damage is modified in cpp for the actor taking damage. i.e. Enemy.cpp CalculateFinalDamageAmount

	HP = FMath::Clamp(HP - Damage, 0.f, MaxHP);

	UE_LOG(LogTemp, Warning, TEXT("Receive Damage. HP: %f"), HP);

}

float UAttributeComponent::GetHealthPercent()
{
	float HPPercent = (HP / MaxHP);

	UE_LOG(LogTemp, Warning, TEXT("GetHealthPercent. HPPercent: %f"), HPPercent);

	return HPPercent;
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


void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

