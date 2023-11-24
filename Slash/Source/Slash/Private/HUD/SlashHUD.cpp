// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"

void ASlashHUD::BeginPlay()
{
	Super::BeginPlay();

	//CREATE OVERLAY
	UWorld* World = GetWorld();
	if (World)
	{
		if (World->GetFirstPlayerController() && SlashOverlayClass)
		{
			SlashOverlay = CreateWidget<USlashOverlay>(World->GetFirstPlayerController(), SlashOverlayClass);
		
			SlashOverlay->AddToViewport();
		}
	}
	
}
