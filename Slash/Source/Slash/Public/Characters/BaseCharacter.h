#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Items/Weapons/WeaponTypes.h"
#include "CharacterTypes.h"
#include "BaseCharacter.generated.h"


class UAnimMontage;
class AWeapon;
class UAttributeComponent;
class UHealthBarComponent;


UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:

	ABaseCharacter();

	virtual void Tick(float DeltaTime) override;

	FString GetName();

	//this is called by the target that is hit in GetHit to tell the attacker that they scored a hit, and execute processed based on that
	void ProcessHitTarget(AActor* TargetHit);

	void DefeatTargetCharacter(ABaseCharacter* CharacterHit);

	//when animations don't complete, end attack state
	UFUNCTION(BlueprintCallable)
	virtual void AbortAttack();


protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Name)
	FString Name = "Default Name";


	virtual void PlaySoundLocal(USoundBase* Sound, const FVector& ImpactPoint);


	/*
	* Combat
	*/



	bool CanAttack();

	bool CanSpecialAttack();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damaged By Properties")
	ABaseCharacter* CombatTarget;

	ECharacterState CharacterState = ECharacterState::ECS_Unarmed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action State")
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool HasMeleeWeapon = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool HasRangedWeapon = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool HasSnipeWeapon = false;

	virtual void MeleeAttack();
	virtual void RangedAttack();
	virtual void SnipeAttack();
	virtual void SpecialAttack();
	virtual void Defend();
	virtual void Dodge();
	virtual void Hide();

	//Used for Death state and for Enemy AI decisions
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	ECombatMode CombatMode = ECombatMode::ECM_OutOfCombat;

	//How close should this follow it's combat target
// Minimum should be 50+ (maybe more...) so the enemy doesn't stand right on player
// ranged focused enemies could be 500+
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	double FollowDistance = 20;
	UFUNCTION(BlueprintCallable)
	void SetFollowDistance();


	//this is for moving closer to a target while in the attack montage
	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	virtual void StartCombat();
	virtual void EndCombat();

	virtual void GetHit_Implementation(const FVector& ImpactPoint, ACharacter* DamageDealer, AWeapon* Weapon);

	UFUNCTION(BlueprintCallable)
	void FinishGetHit();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Death();

	UFUNCTION(BlueprintCallable)
	virtual void SetCombatMode(ECombatMode NewCombatMode);

	UFUNCTION(BlueprintCallable)
	virtual void SetCombatTarget(ABaseCharacter* Target, FString NameOfFunctionCallingThis);

	UFUNCTION(BlueprintCallable)
	virtual void ClearCombatTarget(FString NameOfFunctionCallingThis);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsInCombat = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsRegening = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regen")
	float RegenTickLength = 150.0f;


	virtual bool CanRecover();
	virtual bool CanSee();


	UFUNCTION(BlueprintCallable)
	virtual void Recover(float DeltaTime);

	void RegenStamina();

	void Regen();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dodge)
	float StaminaRequiredToDodge = 15;

	

	UFUNCTION(BlueprintCallable)
	virtual void SetReadyInCombat();

	UFUNCTION(BlueprintCallable)
	virtual void AttachWeapon(const EWeaponType& WeaponType, bool isEquipping);


	UFUNCTION(BlueprintCallable)
	void UnequipWeapon();

	/*
	* Combat FXs and UI
	*/
	virtual void UpdateCombatHUD();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effects")
	FRotator HitParticleRotation = FRotator(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effects")
	FVector HitParticleScale = FVector(1.f, 1.f, 1.f);

	UPROPERTY(EditAnywhere, Category = "Visual Effects")
	UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* DeathSound;


	//this is used to make the character stay on the ground in the pose at the end of the death anim
	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose;

	virtual void SetEquippedWeaponSettings();

	virtual void PlayMontage(UAnimMontage* Montage, const FName& SectionName);


	/*
	* Local Storage of Current Stats
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float MaxHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float RegenPercent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float TPGainPercent = 0.05;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float SpecialAttackTPCost = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float FleeHPPercent = 0.15f;

	/*
	* the most recent damager of this basecharacter
	*/
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damaged By Properties")
	ABaseCharacter* LastAttacker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damaged By Properties")
	AWeapon* LastHitWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damaged By Properties")
	FVector LastHitImpactPoint = FVector();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damaged By Properties")
	FName LastHitDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damaged By Properties")
	float LastDamageAmount;


	/*
	* Components
	*/
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;


/*
* BUFFS
*/

	bool IsInvincible = false;
	bool IsInfinMP = false;
	bool IsInfinTP = false;
	bool IsInfinStam = false;
	float XPMultiplier = 1.f;
	float GoldMultiplier = 1.f;
	float SpeedMultiplier = 1.f;
	float PowerMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	bool IsBuffed = false;

	virtual void BuffTimer(float DeltaTime);

	

	//TODO: if we want to have multiple buffs at once we need one of these for each type of buff?
	UPROPERTY(VisibleAnywhere)
	float BuffTickTimer = 0.0f;
	float LastBuffReceivedDuration = 0.0f;
	

private:

	/*
* Animation Montages
*/

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackSwordMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName> SwordAttackMontageSectionNames;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackHammerMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName> HammerAttackMontageSectionNames;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackRifleMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName> RifleAttackMontageSectionNames;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackPistolMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName> PistolAttackMontageSectionNames;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackBowMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName> BowAttackMontageSectionNames;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	//no section list needed for hit react because it should be standardized for 4 directions.

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName> DeathMontageSectionNames;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName> DodgeMontageSectionNames;

	//UPROPERTY(EditAnywhere, Category = "Montages")
	FName GetRandomSectionName(TArray<FName> MontageList);

	FName CalculateHitReactSectionName(const FVector& ImpactPoint);
	FName CalculateDeathMontageSectionName();

	/*
	* Selected Montage Values
	*/

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages")
	UAnimMontage* AttackMontage;

	//UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName> AttackMontageSectionNames;




	float CalculatePhysicalDamage(float DamageAmount);
	float CalculateMagicalDamage(float DamageAmount);
	float CompareSTRVIT(float AttackerSTR, float DefenderVIT);
	int32 CompareDEXAGI(float AttackerDEX, float DefenderAGI);
	float CompareINTMND(float AttackerINT, float DefenderMND);
	int32 CompareCHR(float AttackerCHR, float DefenderCHR);

	bool CheckCritical(const FVector& ImpactPoint);
	void IncreaseTP();


	FName WeaponTypeToSocketFName(const EWeaponType& WeaponType, bool isEquipping);

	float RegenTickTimer = 0.0f;



//Getters and Setters
public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE ECombatMode GetCombatMode() const { return CombatMode; }

	FORCEINLINE EDeathPose GetDeathPose() const { return DeathPose; }

	//FORCEINLINE void SetCharacterState(ECharacterState NewCharacterState) { CharacterState = NewCharacterState; }

	FORCEINLINE EActionState GetActionState() const { return ActionState; }

	FORCEINLINE UAttributeComponent* GetAttributes() const { return Attributes; }

	FORCEINLINE AWeapon* GetWeapon() const { return EquippedWeapon; }

	FORCEINLINE void SetInCombat(bool NewIsInCombat) { IsInCombat = NewIsInCombat; }

	FORCEINLINE float GetPowerMultiplier() const { return PowerMultiplier; }

};
