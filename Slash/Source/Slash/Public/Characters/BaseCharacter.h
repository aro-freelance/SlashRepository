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

//@Yelsa : Bug - If a either character is interrupted mid-attack,
//  their actionstate never goes from attacking to unoccupied. This results in the character locking up.
// TODO: fix this by setting unoccupied actionstate when animation is interrupted 
//  OR Set it when hit?


UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:

	ABaseCharacter();

	virtual void Tick(float DeltaTime) override;


	virtual void GetHit_Implementation(const FVector& ImpactPoint, ACharacter* DamageDealer, AWeapon* Weapon);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Death();

	FString GetName();

	UFUNCTION(BlueprintCallable)
	virtual void AttachWeapon(const EWeaponType& WeaponType, bool isEquipping);

	FName WeaponTypeToSocketFName(const EWeaponType& WeaponType, bool isEquipping);

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Name)
	FString Name = "Default Name";


	/*
	* Attack
	*/
	
	bool CanAttack();
	FString BuildCharacterStateString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	AWeapon* EquippedWeapon;


	ECharacterState CharacterState = ECharacterState::ECS_Unarmed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action State")
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool HasMeleeWeapon = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool HasRangedWeapon = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool HasSnipeWeapon = false;

	/*
	* Combat
	*/

	virtual void MeleeAttack();
	virtual void RangedAttack();
	virtual void SnipeAttack();
	virtual void SpecialAttack();
	virtual void Defend();
	virtual void Dodge();
	virtual void Hide();

	virtual void StartCombat();
	virtual void EndCombat();

	void IncreaseTP();

	float CalculatePhysicalDamage(float DamageAmount);
	float CalculateMagicalDamage(float DamageAmount);
	float CompareSTRVIT(float AttackerSTR, float DefenderVIT);
	int32 CompareDEXAGI(float AttackerDEX, float DefenderAGI);
	float CompareINTMND(float AttackerINT, float DefenderMND);
	int32 CompareCHR(float AttackerCHR, float DefenderCHR);

	bool CheckCritical(const FVector& ImpactPoint);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsInCombat = false;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsRegening = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regen")
	float RegenTickLength = 150.0f;
	UFUNCTION(BlueprintCallable)
	void Recover(float DeltaTime);

	//when animations don't complete, end attack state
	UFUNCTION(BlueprintCallable)
	virtual void AbortAttack();

	UFUNCTION(BlueprintCallable)
	void UnequipWeapon();

	/*
	* Combat FXs and UI
	*/
	void UpdateCombatHUD();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effects")
	FRotator HitParticleRotation = FRotator(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effects")
	FVector HitParticleScale = FVector(1.f, 1.f, 1.f);

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Visual Effects")
	UParticleSystem* HitParticles;

	/*
	* Animation Montages
	*/

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackSwordMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackHammerMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackRifleMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackPistolMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackBowMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	//this is a placeholder for the currently active AttackMontage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose;

	virtual void PlayAttackMontage(const EWeaponType& WeaponSize);
	virtual void PlayHitReactMontage(const FName& SectionName);
	virtual void PlayDeathMontage(const FName& SectionName);


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
	ABaseCharacter* CombatTarget;
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

private:

	FName CalculateHitReactSectionName(const FVector& ImpactPoint);
	FName CalculateDeathMontageSectionName();



	float RegenTickTimer = 0.0f;


//Getters and Setters
public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

	//FORCEINLINE void SetCharacterState(ECharacterState NewCharacterState) { CharacterState = NewCharacterState; }

	FORCEINLINE EActionState GetActionState() const { return ActionState; }

	FORCEINLINE UAttributeComponent* GetAttributes() const { return Attributes; }

	FORCEINLINE AWeapon* GetWeapon() const { return EquippedWeapon; }

};
