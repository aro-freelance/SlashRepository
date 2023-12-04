#pragma once

UENUM(BlueprintType)
enum class EPatrolType : uint8
{
	EPT_FullRandom UMETA(DisplayName = "Full Random Patrol"),
	EPT_RandomInSet UMETA(DisplayName = "Random In Set"),
	EPT_SetOrderLoopEnd UMETA(DisplayName = "Set Order Loop End"),
	EPT_SetOrderReverseEnd UMETA(DisplayName = "Set Order Reverse End")
};



UENUM(BlueprintType)
enum class EEnemyWeapon : uint8
{
	EEW_Unarmed UMETA(DisplayName = "Unarmed"),
	EEW_Sword UMETA(DisplayName = "Sword"),
	EEW_GreatHammer UMETA(DisplayName = "Great Hammer"),
	EEW_Rifle UMETA(DisplayName = "Rifle"),
	EEW_Pistol UMETA(DisplayName = "Pistol"),
	EEW_Bow UMETA(DisplayName = "Bow"),
	EEW_HandToHand UMETA(DisplayName = "HandToHand"),
	EEW_Bite UMETA(DisplayName = "Bite"),
	EEW_Claw UMETA(DisplayName = "Claw")
};