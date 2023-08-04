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