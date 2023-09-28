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

UENUM(BlueprintType)
enum class EPatrolType : uint8
{
	EPT_FullRandom UMETA(DisplayName = "Full Random Patrol"),
	EPT_RandomInSet UMETA(DisplayName = "Random In Set"),
	EPT_SetOrderLoopEnd UMETA(DisplayName = "Set Order Loop End"),
	EPT_SetOrderReverseEnd UMETA(DisplayName = "Set Order Reverse End")
};