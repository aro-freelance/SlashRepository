#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
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
class USphereComponent;


//TODO: 
// 1. DOWNLOAD ATTACK ANIMATIONS AGAIN FOR ECHO. 
// 2. HANDLE ROOT ANIM WITH BLENDER. 
// 3. Reimport 
// 4. Add to montages and reintegrate 
// 5. Test


UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Death() override;

protected:
	virtual void BeginPlay() override;

	/*
	* Input
	*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* SlashCharacterContext;

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
	//input = hold equip

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CharacterLockOnAction;
	void LockOnToEnemy(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CharacterToggleLockOnAction;
	void ToggleEnemyToLock(const FInputActionValue& Value);


	/*
	* Equip
	*/
	void DropWeapon(const FInputActionValue& Value);
	void EquipNewWeapon();
	void EquipAnimation();
	void UnEquipAnimation();
	bool CanDisarm();
	bool CanArm();
	bool CanDropWeapon();
	bool HasWeapon();


	//void PlayEquipMontage(const FName& SectionName);
	FName WeaponSizeToEquipMontageFName(const EWeaponType& WeaponSize, bool isEquipping);
	
	ECharacterState WeaponSizeToCharacterState(const EWeaponType& WeaponSize);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock On")
	bool HasEnemyLockedOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock On")
	float LockOnRadius = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock On")
	float TargetLockDistance = 600.f;

	

private:


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


	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipSwordMontage;


//Getters and Setters
public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

};
