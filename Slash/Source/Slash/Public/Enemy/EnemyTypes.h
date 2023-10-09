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
enum class EWeaponType : uint8
{
	EWT_Unarmed UMETA(DisplayName = "Unarmed"),
	EWT_Sword UMETA(DisplayName = "Sword"),
	EWT_GreatHammer UMETA(DisplayName = "Great Hammer"),
	EWT_Rifle UMETA(DisplayName = "Rifle"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_Bow UMETA(DisplayName = "Bow")
};