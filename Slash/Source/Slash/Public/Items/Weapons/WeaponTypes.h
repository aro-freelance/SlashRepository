#pragma once

UENUM(BlueprintType)
enum class EWeaponSize : uint8
{
	EWS_OneHanded UMETA(DisplayName = "One Handed"),
	EWS_TwoHanded UMETA(DisplayName = "Two Handed"),
	EWS_Bow UMETA(DisplayName = "Bow"),
	EWS_Rifle UMETA(DisplayName = "Rifle"),
	EWS_Pistol UMETA(DisplayName = "Pistol")
};

UENUM(BlueprintType)
enum class EWeaponCollisionState : uint8
{
	EWS_CollisionOn UMETA(DisplayName = "Collision On"),
	EWS_CollisionOff UMETA(DisplayName = "Collision Off")
};