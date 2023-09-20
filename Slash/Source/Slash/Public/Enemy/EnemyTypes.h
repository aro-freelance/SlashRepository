#pragma once

UENUM(BlueprintType)
enum class EDeathPose : uint8
{
	EDP_Alive UMETA(DisplayName = "Alive"),
	EDP_FallBack UMETA(DisplayName = "Death Fall Back"),
	EDP_FallForward UMETA(DisplayName = "Death Fall Forward"),
	EDP_Shoulder UMETA(DisplayName = "Death Shoulder"),
	EDP_Sweep UMETA(DisplayName = "Death Sweep")
};