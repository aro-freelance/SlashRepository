#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unarmed UMETA(DisplayName = "Unarmed"),
	ECS_EquippedOneHanded UMETA(DisplayName = "Equipped One Handed"),
	ECS_EquippedTwoHanded UMETA(DisplayName = "Equipped Two Handed")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_Equipping UMETA(DisplayName = "Equipping")
};

UENUM(BlueprintType)
enum class EDeathPose : uint8
{
	EDP_FallBack UMETA(DisplayName = "Death Fall Back"),
	EDP_FallForward UMETA(DisplayName = "Death Fall Forward"),
	EDP_Shoulder UMETA(DisplayName = "Death Shoulder"),
	EDP_Sweep UMETA(DisplayName = "Death Sweep")
};

UENUM(BlueprintType)
enum class ECombatMode : uint8
{
	ECM_OutOfCombat UMETA(DisplayName = "OutOfCombat"),
	ECM_Chasing UMETA(DisplayName = "Chasing"),
	ECM_MeleeAttacking UMETA(DisplayName = "Melee Attacking"),
	ECM_RangeAttacking UMETA(DisplayName = "Range Attacking"),
	ECM_SnipeAttacking UMETA(DisplayName = "Snipe Attacking"),
	ECM_SpecialAttacking UMETA(DisplayName = "Special Attacking"),
	ECM_Defending UMETA(DisplayName = "Defending"),
	ECM_Dodging UMETA(DisplayName = "Dodging"),
	ECM_Fleeing UMETA(DisplayName = "Fleeing"),
	ECM_Hiding UMETA(DisplayName = "Hiding"),
	ECM_Dead UMETA(DisplayName = "Dead")
};