// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "Items/Weapons/WeaponTypes.h"
#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UGroomComponent;
class AItem;
class UAnimMontage;
class AWeapon;
class UAttributeComponent;


UCLASS()
class SLASH_API ASlashCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FString GetName();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* SlashCharacterContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Name")
	FString CharacterName = "Default Character Name";

	/*
	* Player Input Callbacks
	*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CharacterMoveAction;

	void Move(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CharacterLookAction;

	void Look(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CharacterJumpAction;

	void Jump(const FInputActionValue& Value);


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CharacterDodgeAction;

	void Dodge(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CharacterEquipAction;

	void Equip(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CharacterAttackAction;

	void Attack(const FInputActionValue& Value);


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CharacterDropWeaponAction;

	void DropWeapon(const FInputActionValue& Value);

	void EquipNewWeapon();
	void EquipAnimation();
	void UnEquipAnimation();

	

	/*
	* Play montage functions
	*/

	void PlayAttackMontage(const EWeaponSize& WeaponSize);
	bool CanAttack();

	void PlayEquipMontage(const FName& SectionName);
	bool CanDisarm();
	bool CanArm();
	bool HasWeapon();

	bool CanDropWeapon();

	FName WeaponSizeToSocketFName(const EWeaponSize& WeaponSize, bool isEquipping);
	FName WeaponSizeToEquipMontageFName(const EWeaponSize& WeaponSize, bool isEquipping);
	ECharacterState WeaponSizeToCharacterState(const EWeaponSize& WeaponSize);

	UFUNCTION(BlueprintCallable)
	void AttachWeapon(const EWeaponSize& WeaponSize, bool isEquipping);


	FString BuildStateString();


private:

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	//UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	ECharacterState CharacterState = ECharacterState::ECS_Unarmed;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

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
	UAnimMontage* EquipSwordMontage;


//Getters and Setters
public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	
	//FORCEINLINE void SetCharacterState(ECharacterState NewCharacterState) { CharacterState = NewCharacterState; }

	FORCEINLINE EActionState GetActionState() const { return ActionState; }

	FORCEINLINE UAttributeComponent* GetAttributes() const { return Attributes; }

	FORCEINLINE AWeapon* GetWeapon() const { return EquippedWeapon; }

};
