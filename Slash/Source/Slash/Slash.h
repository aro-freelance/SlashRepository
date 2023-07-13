// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define DrawSphere(Location) if(GetWorld()) DrawDebugSphere(GetWorld(), Location, 30.f, 12, FColor::Blue, true);

#define DebugLine(Location, ForwardVector) if(GetWorld()) DrawDebugLine(GetWorld(), Location, Location + (ForwardVector * 100), FColor::Red, true);

