#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_OneHanded UMETA(DisplayName = "One Handed"),
	EWT_TwoHanded UMETA(DisplayName = "Two Handed"),
	EWT_Bow UMETA(DisplayName = "Bow"),
	EWT_Rifle UMETA(DisplayName = "Rifle"),
	EWT_Pistol UMETA(DisplayName = "Pistol")
};

UENUM(BlueprintType)
enum class EWeaponCollisionState : uint8
{
	EWS_CollisionOn UMETA(DisplayName = "Collision On"),
	EWS_CollisionOff UMETA(DisplayName = "Collision Off")
};