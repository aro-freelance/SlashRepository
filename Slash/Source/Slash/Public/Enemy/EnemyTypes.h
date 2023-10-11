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
enum class ECombatMode : uint8
{
	ECM_MeleeAttacking UMETA(DisplayName = "Melee Attacking"),
	ECM_RangeAttacking UMETA(DisplayName = "Range Attacking"),
	ECM_SnipeAttacking UMETA(DisplayName = "Snipe Attacking"),
	ECM_SpecialAttacking UMETA(DisplayName = "Special Attacking"),
	ECM_Defending UMETA(DisplayName = "Defending"),
	ECM_Dodging UMETA(DisplayName = "Dodging"),
	ECM_Chasing UMETA(DisplayName = "Chasing"),
	ECM_Fleeing UMETA(DisplayName = "Fleeing"),
	ECM_Hiding UMETA(DisplayName = "Hiding"),
	ECM_OutOfCombat UMETA(DisplayName = "OutOfCombat")
};

UENUM(BlueprintType)
enum class EEnemyWeapon : uint8
{
	EEW_Unarmed UMETA(DisplayName = "Unarmed"),
	EEW_Sword UMETA(DisplayName = "Sword"),
	EEW_GreatHammer UMETA(DisplayName = "Great Hammer"),
	EEW_Rifle UMETA(DisplayName = "Rifle"),
	EEW_Pistol UMETA(DisplayName = "Pistol"),
	EEW_Bow UMETA(DisplayName = "Bow")
};