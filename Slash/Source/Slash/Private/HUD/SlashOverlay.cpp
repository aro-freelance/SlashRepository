// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USlashOverlay::SetHealthBarPercent(float Percent)
{
	if (HPProgressBar)
	{
		HPProgressBar->SetPercent(Percent);
	}
}

void USlashOverlay::SetTPBarPercent(float Percent)
{
	if (TPProgressBar)
	{
		TPProgressBar->SetPercent(Percent);
	}
}

void USlashOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void USlashOverlay::SetGoldText(int32 Amount)
{
	if (GoldAmountText)
	{
		FText GoldFText = FText::FromString(FString::Printf(TEXT("%d"), Amount));
		GoldAmountText->SetText(GoldFText);
	}
}

void USlashOverlay::SetSoulsText(int32 Amount)
{
	if (SoulAmountText)
	{
		FText SoulsFText = FText::FromString(FString::Printf(TEXT("%d"), Amount));
		SoulAmountText->SetText(SoulsFText);
	}
}
