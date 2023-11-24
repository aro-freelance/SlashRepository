// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "SlashOverlay.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()


public:

	void SetHealthBarPercent(float Percent);
	void SetTPBarPercent(float Percent);

	void SetGoldText(int32 Amount);
	void SetSoulsText(int32 Amount);

private:

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* TPProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldAmountText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SoulAmountText;
	
};
