#pragma once


UENUM(BlueprintType)
enum class EPotionType : uint8
{
	EPT_Health,
	EPT_Mana,
	EPT_TP,
	EPT_Stamina,
	EPT_Buff
};


UENUM(BlueprintType)
enum class EBuffType : uint8
{
	EBT_None,
	EBT_Invincible,
	EBT_InfinMP,
	EBT_InfinTP,
	EBT_InfinStam,
	EBT_XPMultiplier,
	EBT_GoldMultiplier,
	EBT_SpeedBoost,
	EBT_PowerBoost
	
};