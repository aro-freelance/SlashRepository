// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"
#include "HUD/HealthBar.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Internationalization/Text.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{

	if (HealthBarWidget == nullptr)
	{
		HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	}

	if (HealthBarWidget && HealthBarWidget->HealthBar)
	{
		HealthBarWidget->HealthBar->SetPercent(Percent);
	}

}

void UHealthBarComponent::SetHealthText(float Amount)
{
	if (HealthBarWidget == nullptr)
	{
		HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	}
	if (HealthBarWidget && HealthBarWidget->HealthBar)
	{

		Amount = FMath::RoundToInt32(Amount * 100);
		FString AmountString = FString::SanitizeFloat(Amount) + "%";

		HealthBarWidget->HPText->SetText(FText::FromString(AmountString));
	}

}
