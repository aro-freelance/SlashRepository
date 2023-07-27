#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unarmed UMETA(DisplayName = "Unarmed"),
	ECS_EquippedOneHanded UMETA(DisplayName = "Equipped One Handed"),
	ECS_EquippedTwoHanded UMETA(DisplayName = "Equipped Two Handed")
};

