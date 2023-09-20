// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

/**
 * 
 */
UCLASS()

class SLASH_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	void SetHealthPercent(float Percent);

	void SetHealthText(float Amount);

	void SetNameText(FString Name);

private:
	UPROPERTY()
	class UHealthBar* HealthBarWidget;

};
