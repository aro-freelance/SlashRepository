// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Items/RecoveryPickup.h"
#include "Components/AttributeComponent.h"
#include "Items/Weapons/Weapon.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "Components/SkeletalMeshComponent.h"
#include "Enemy/Enemy.h"

#include "HUD/SlashOverlay.h"
#include "HUD/SlashHUD.h"


ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//don't rotate the character when the camera moves. in editor set use pawn rotation on camera boom to move camera instead.
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 170.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	Tags.Add(FName("Player"));

}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{

		if (ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD()))
		{
			SlashOverlay = SlashHUD->GetSlashOverlay();
			
			//initialize the HUD
			UpdateCombatHUD();
		}


		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SlashCharacterContext, 0);
		}
	}

	

}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	if (IsBusy()) { return; }
	
	if (ActionState != EActionState::EAS_Unoccupied) return;

	if (SpeedMultiplier)
	{
		//TODO: set speed to higher amount. 
		//Turn this off by returning it to default in the function that turns buff off. 
		// (so we should store the movement speed before we change it)
	}

	//get controller movement input
	const FVector2D MovementVector = Value.Get<FVector2D>();
	//get controller camera input
	const FRotator Rotation = Controller->GetControlRotation();

	//direction rotation of camera accounting only for movement on the ground 
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	//move the character based on camera rotation
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);

}

void ASlashCharacter::Look(const FInputActionValue& Value)
{

	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisValue.X);
	AddControllerPitchInput(LookAxisValue.Y);

}

void ASlashCharacter::Jump(const FInputActionValue& Value)
{
	if (IsBusy()) { return; }

	UE_LOG(LogTemp, Warning, TEXT("Jump pressed"));

	Super::Jump();

}

void ASlashCharacter::Dodge(const FInputActionValue& Value)
{
	if (IsBusy()) { return; }

	UE_LOG(LogTemp, Warning, TEXT("Dodge pressed"));


	if (Attributes)
	{
		if (CanDodge())
		{
			if (!IsInfinStam)
			{
				Attributes->UseStamina(StaminaRequiredToDodge);
			}
			
			ABaseCharacter::Dodge();
			UpdateCombatHUD();
		}
		else
		{
			//TODO: indicate to player than they are low on stamina
			UE_LOG(LogTemp, Warning, TEXT("Not Enough Stamina. %f"), Attributes->GetStamina());
		}
	}
	

}

void ASlashCharacter::Equip(const FInputActionValue& Value)
{
	if (IsBusy()) { return; }

	if (CanDisarm())
	{
		ActionState = EActionState::EAS_Equipping;

		UnEquipAnimation();
	}
	else if (CanArm())
	{
		ActionState = EActionState::EAS_Equipping;

		if (!HasWeapon())
		{
			EquipNewWeapon();
		}
		else 
		{
			EquipAnimation();
		}
		
	}

	if (ActionState == EActionState::EAS_Unoccupied) { UE_LOG(LogTemp, Warning, TEXT("equip press finished: unoccupied")); }
	if (ActionState == EActionState::EAS_Attacking) { UE_LOG(LogTemp, Warning, TEXT("equip press finished: attacking")); }
	if (ActionState == EActionState::EAS_Equipping) { UE_LOG(LogTemp, Warning, TEXT("equip press finished: equipping")); }

}

void ASlashCharacter::Death()
{
	Super::Death();

	//TODO: Handle Player Character Death with GameOver or with some sort of downed mechanic

}

void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes)
	{
		float SoulValue = Soul->GetValue();
		
		if (XPMultiplier)
		{
			SoulValue = SoulValue * (1 + XPMultiplier);
		}

		float StoredSouls = Attributes->GetSouls();

		UE_LOG(LogTemp, Warning, TEXT("add soul. stored %f. new  %f"), StoredSouls, SoulValue);

		Attributes->SetSouls(StoredSouls + SoulValue);

		UpdateCombatHUD();
	}
}

void ASlashCharacter::UseHealthPotion(ARecoveryPickup* Potion)
{
	UE_LOG(LogTemp, Warning, TEXT("health potion Function."));
	
	if (Attributes)
	{
		float PotionPower = Potion->GetPotionPower();
		float LocalMaxHP = Attributes->GetMaxHP();
		float HealAmount = PotionPower * LocalMaxHP;

		float Amount = FMath::Clamp((Attributes->GetHP() + HealAmount), 0.0f, LocalMaxHP);

		Attributes->SetHP(Amount);
		UpdateCombatHUD();
	}

}

void ASlashCharacter::UseManaPotion(ARecoveryPickup* Potion)
{
	if (Attributes)
	{
		float PotionPower = Potion->GetPotionPower();
		float MaxMP = Attributes->GetMaxMP();
		float HealAmount = PotionPower * MaxMP;

		float Amount = FMath::Clamp((Attributes->GetMP() + HealAmount), 0.0f, MaxMP);

		Attributes->SetMP(Amount);
		UpdateCombatHUD();
	}
}

void ASlashCharacter::UseTPPotion(ARecoveryPickup* Potion)
{
	if (Attributes)
	{
		float PotionPower = Potion->GetPotionPower();
		float MaxTP = Attributes->GetMaxTP();
		float HealAmount = PotionPower * MaxTP;

		float Amount = FMath::Clamp((Attributes->GetTP() + HealAmount), 0.0f, MaxTP);

		Attributes->SetTP(Amount);
		UpdateCombatHUD();
	}
}

void ASlashCharacter::UseStaminaPotion(ARecoveryPickup* Potion)
{
	if (Attributes)
	{
		float PotionPower = Potion->GetPotionPower();
		float MaxStamina = Attributes->GetMaxStamina();
		float HealAmount = PotionPower * MaxStamina;

		float Amount = FMath::Clamp((Attributes->GetStamina() + HealAmount), 0.0f, MaxStamina);

		Attributes->SetStamina(Amount);
		UpdateCombatHUD();
	}
}

void ASlashCharacter::UseBuffPotion(ARecoveryPickup* Potion)
{
	if (Attributes && SlashOverlay)
	{
		float PotionPower = Potion->GetPotionPower();
		EBuffType BuffType = Potion->GetBuffType();
		float Duration = Potion->GetPotionDuration();
		FString DisplayText = "";

		switch(BuffType)
		{
		case EBuffType::EBT_None:
			break;
		case EBuffType::EBT_Invincible:
			IsInvincible = true;
			DisplayText = "Invincible Buff!";
			break;
		case EBuffType::EBT_InfinMP:
			IsInfinMP = true;
			DisplayText = "Infinite MP Buff!";
			break;
		case EBuffType::EBT_InfinTP:
			IsInfinTP = true;
			DisplayText = "Infinite TP Buff!";
			break;
		case EBuffType::EBT_InfinStam:
			IsInfinStam = true;
			DisplayText = "Infinite Stamina Buff!";
			break;
		case EBuffType::EBT_XPMultiplier:
			XPMultiplier = XPMultiplier * (1 + PotionPower);
			DisplayText = "Experience Buff!";
			break;
		case EBuffType::EBT_GoldMultiplier:
			GoldMultiplier = GoldMultiplier * (1 + PotionPower);
			DisplayText = "Gold Buff!";
			break;
		case EBuffType::EBT_SpeedBoost:
			SpeedMultiplier = SpeedMultiplier * (1 + PotionPower);
			DisplayText = "Speed Buff!";
			break;
		case EBuffType::EBT_PowerBoost:
			PowerMultiplier = PowerMultiplier * (1 + PotionPower);
			DisplayText = "Power Buff!";
			break;

		}

		//TODO: if visual effect is added turn it on here
		SlashOverlay->SetCenterPopupText(DisplayText);


		//TODO: handle picking up another buff by either: A. canceling previous. or B. Keeping separate timers for each type of buff?
		if (BuffType != EBuffType::EBT_None)
		{
			IsBuffed = true;
			BuffTickTimer = 0.0f;
			LastBuffReceivedDuration = Duration;
		}
	}
}



void ASlashCharacter::ClearCenterPopupText()
{
	if (SlashOverlay)
	{
		SlashOverlay->SetCenterPopupText("");
	}
}


bool ASlashCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied && 
		CharacterState != ECharacterState::ECS_Unarmed;
}

bool ASlashCharacter::CanArm()
{

	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unarmed;
}

bool ASlashCharacter::HasWeapon()
{
	bool hasStoredWeapon = false;
	if (EquippedWeapon) 
	{
		hasStoredWeapon = true;
	}

	return hasStoredWeapon;
}

void ASlashCharacter::EquipNewWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("equip weapon, no weapon stored"));

	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("overlapping weapon = good"));

		EquippedWeapon = OverlappingWeapon;

		EquippedWeapon->SetOwner(this);
		EquippedWeapon->SetInstigator(this);

		EWeaponType WeaponSize = EquippedWeapon->GetWeaponType();

		CharacterState = WeaponSizeToCharacterState(WeaponSize);

		AttachWeapon(WeaponSize, true);

		EquippedWeapon->PlayWeaponPickupSound();

	}

	/*this is turned off here because there is no associated anim playing at this time
	//TODO: since this is turned off here,
	if the player spams the equip button the weapon will be picked up and then sheathed
	this is undesirable.
	if possible we should make it so that when the weapon is intitially picked up
	it cannot be sheathed until a delay has passed
	*/
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::EquipAnimation()
{
	EWeaponType WeaponSize = EquippedWeapon->GetWeaponType();
	FName MontageName = WeaponSizeToEquipMontageFName(WeaponSize, true);
	CharacterState = WeaponSizeToCharacterState(WeaponSize);

	PlayMontage(EquipSwordMontage, MontageName);

	//AttachWeapon is called in blueprints when anim notify happens at a certain point in the animation
	
}

void ASlashCharacter::UnEquipAnimation()
{

	EWeaponType WeaponSize = EquippedWeapon->GetWeaponType();
	FName MontageName = WeaponSizeToEquipMontageFName(WeaponSize, false);

	CharacterState = ECharacterState::ECS_Unarmed;

	EquippedWeapon->SetOwner(nullptr);
	EquippedWeapon->SetInstigator(nullptr);

	PlayMontage(EquipSwordMontage, MontageName);

	//AttachWeapon is called in blueprints when anim notify happens at a certain point in the animation
	
}

ECharacterState ASlashCharacter::WeaponSizeToCharacterState(const EWeaponType& WeaponSize)
{
	ECharacterState NewCharacterState = ECharacterState();

	//TODO: add cases for other weapon types

	switch (WeaponSize)
	{
	case EWeaponType::EWT_OneHanded:
		NewCharacterState = ECharacterState::ECS_EquippedOneHanded;
		break;
	case EWeaponType::EWT_TwoHanded:
		NewCharacterState = ECharacterState::ECS_EquippedTwoHanded;
		break;
	case EWeaponType::EWT_Rifle:
		NewCharacterState = ECharacterState::ECS_EquippedOneHanded;
		break;
	case EWeaponType::EWT_Pistol:
		NewCharacterState = ECharacterState::ECS_EquippedOneHanded;
		break;
	case EWeaponType::EWT_Bow:
		NewCharacterState = ECharacterState::ECS_EquippedOneHanded;
		break;
	default:
		break;
	}

	return NewCharacterState;
}

void ASlashCharacter::UpdateCombatHUD()
{
	if (SlashOverlay && Attributes)
	{
		float HPP = Attributes->GetHealthPercent();
		float TPP = Attributes->GetTPPercent();
		float StaminaP = Attributes->GetStaminaPercent();
		float Gold = Attributes->GetGold();
		float Souls = Attributes->GetSouls();

		UE_LOG(LogTemp, Warning, 
			TEXT("UpdateCombatHUD. HPBarPercent: %f. TPBarPercent: %f. StaminaP: %f. GoldText: %f. SoulsText: %f."),
			HPP, TPP, StaminaP, Gold, Souls);

		SlashOverlay->SetHealthBarPercent(HPP);
		SlashOverlay->SetTPBarPercent(TPP);
		SlashOverlay->SetStaminaBarPercent(StaminaP);
		SlashOverlay->SetGoldText(Gold);
		SlashOverlay->SetSoulsText(Souls);
	}
	else
	{
		if (SlashOverlay && !Attributes)
		{
			UE_LOG(LogTemp, Warning, TEXT("Update Combat HUD failed because attributes don't exist."));
		}
		if (Attributes && !SlashOverlay)
		{
			UE_LOG(LogTemp, Warning, TEXT("Update Combat HUD failed because overlay doesn't exist."));
		}
		if(!SlashOverlay && !Attributes)
		{
			UE_LOG(LogTemp, Warning, TEXT("Update Combat HUD failed because overlay and attributes both don't exist."));
		}
		
	}
}

void ASlashCharacter::SetReadyInCombat()
{
	Super::SetReadyInCombat();

	if (IsInCombat)
	{
		CombatMode = ECombatMode::ECM_ReadyInCombat;
	}
	else
	{
		CombatMode = ECombatMode::ECM_OutOfCombat;
	}
}

bool ASlashCharacter::IsBusy()
{
	bool isBusy = false;

	if (CombatMode == ECombatMode::ECM_Dead ||
		CombatMode == ECombatMode::ECM_MeleeAttacking ||
		CombatMode == ECombatMode::ECM_RangeAttacking ||
		CombatMode == ECombatMode::ECM_SnipeAttacking ||
		CombatMode == ECombatMode::ECM_SpecialAttacking ||
		CombatMode == ECombatMode::ECM_Dodging)
	{
		isBusy = true;
	}

	return isBusy;
}

bool ASlashCharacter::CanDodge()
{
	bool IsDodgeReady = false;
	if (Attributes->GetStamina() >= StaminaRequiredToDodge)
	{
		IsDodgeReady = true;
	}

	if (IsInfinStam)
	{
		IsDodgeReady = true;
	}
	
	return IsDodgeReady;
}





//this is called in treasure OnSphereOverlap
void ASlashCharacter::AddTreasure(ATreasure* Treasure)
{
	if (Attributes)
	{
		int32 CurrentGold = Attributes->GetGold();
		int32 TreasureAmount = Treasure->GetGoldAmount();

		if (GoldMultiplier)
		{
			TreasureAmount = TreasureAmount * (1 + GoldMultiplier);
		}

		Attributes->SetGold(CurrentGold + TreasureAmount);

		UpdateCombatHUD();
	}
	
	//TODO add something to inventory?

}



FName ASlashCharacter::WeaponSizeToEquipMontageFName(const EWeaponType& WeaponSize, bool isEquipping)
{
	FName MontageName = FName();

	//TODO: add cases for other weapon types

	switch (WeaponSize)
	{
	case EWeaponType::EWT_OneHanded:
		if (isEquipping) 
		{
			MontageName = FName("Equip");
		}
		else 
		{
			MontageName = FName("Unequip");
		}

		break;
	case EWeaponType::EWT_TwoHanded:
		if (isEquipping)
		{
			MontageName = FName("EquipTwoHanded");
		}
		else
		{
			MontageName = FName("UnequipTwoHanded");
		}

		break;
	case EWeaponType::EWT_Rifle:
		if (isEquipping)
		{
			MontageName = FName("EquipRifle");
		}
		else
		{
			MontageName = FName("UnequipRifle");
		}

		break;
	case EWeaponType::EWT_Pistol:
		if (isEquipping)
		{
			MontageName = FName("EquipPistol");
		}
		else
		{
			MontageName = FName("UnequipPistol");
		}

		break;
	case EWeaponType::EWT_Bow:
		if (isEquipping)
		{
			MontageName = FName("EquipBow");
		}
		else
		{
			MontageName = FName("UnequipBow");
		}

		break;
	default:
		break;

	}

	return MontageName;
}


void ASlashCharacter::Attack(const FInputActionValue& Value)
{
	if (IsBusy()) { return; }

	if (CanAttack())
	{
		UE_LOG(LogTemp, Warning, TEXT("Can attack"));

		EWeaponType WeaponType = EquippedWeapon->GetWeaponType();

		switch (WeaponType)
		{
		case EWeaponType::EWT_OneHanded:
			MeleeAttack();
			break;
		case EWeaponType::EWT_TwoHanded:
			MeleeAttack(); //TODO: two handed melee?
			break;
		case EWeaponType::EWT_Rifle:
			RangedAttack();
			break;
		case EWeaponType::EWT_Pistol:
			RangedAttack();
			break;
		case EWeaponType::EWT_Bow:
			RangedAttack();
			break;
		default:
			break;

		}

	}

}

void ASlashCharacter::LockOnToEnemy(const FInputActionValue& Value)
{

	UE_LOG(LogTemp, Warning, TEXT("Lock On pressed"));

	if (HasEnemyLockedOn)
	{
		HasEnemyLockedOn = false;

		//TODO: unlock
		CombatTarget = nullptr;

		UE_LOG(LogTemp, Warning, TEXT("unlocked"));
	}
	else
	{

		FVector CameraForwardVector = CameraBoom->GetTargetRotation().Vector();

		FVector EndVector = (CameraForwardVector * TargetLockDistance) + GetActorLocation();

		TArray<TEnumAsByte<EObjectTypeQuery>> CharactersInLockOnRadius;
		CharactersInLockOnRadius.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);
		FHitResult OutHit;

		UKismetSystemLibrary::SphereTraceSingleForObjects(
			GetWorld(),
			GetActorLocation(),
			EndVector,
			LockOnRadius,
			CharactersInLockOnRadius,
			false, // bool bTraceComplex
			ActorsToIgnore,
			EDrawDebugTrace::None,
			OutHit,
			true // bool bIgnoreSelf
		);

		//if an character is hit
		ABaseCharacter* HitActor = Cast<ABaseCharacter>(OutHit.GetActor());
		if (HitActor)
		{
			//store as combat target
			if (!CombatTarget) { UE_LOG(LogTemp, Warning, TEXT("setting new combat target: %s"), *HitActor->GetName()); }
			CombatTarget = HitActor;

			HasEnemyLockedOn = true;
		}
		
	}

}

void ASlashCharacter::ToggleEnemyToLock(const FInputActionValue& Value)
{

	UE_LOG(LogTemp, Warning, TEXT("Toggle Lock pressed"));

	//TODO: if locked on to enemy, check all enemies in range, add to array, 
	// lock on to the next possible enemy in array

}




void ASlashCharacter::DropWeapon(const FInputActionValue& Value)
{
	if (IsBusy()) { return; }

	UE_LOG(LogTemp, Warning, TEXT("drop weapon pressed"));

	if (EquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("equipped weapon true"));

		if (CanDropWeapon())
		{
			UnequipWeapon();
	
		}
	}
}


bool ASlashCharacter::CanDropWeapon()
{

	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unarmed &&
		EquippedWeapon->GetItemState() == EItemState::EIS_Held;
}



void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



	if (CombatTarget && HasEnemyLockedOn)
	{
		//turn camera toward locked on target
		Controller->SetControlRotation(
			UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CombatTarget->GetActorLocation()));
	}

}


void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(CharacterMoveAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(CharacterLookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);

		EnhancedInputComponent->BindAction(CharacterJumpAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Jump);
		EnhancedInputComponent->BindAction(CharacterDodgeAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Dodge);
		EnhancedInputComponent->BindAction(CharacterEquipAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Equip);
		EnhancedInputComponent->BindAction(CharacterAttackAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Attack);
		EnhancedInputComponent->BindAction(CharacterDropWeaponAction, ETriggerEvent::Triggered, this, &ASlashCharacter::DropWeapon);

		EnhancedInputComponent->BindAction(CharacterLockOnAction, ETriggerEvent::Triggered, this, &ASlashCharacter::LockOnToEnemy);
		EnhancedInputComponent->BindAction(CharacterToggleLockOnAction, ETriggerEvent::Triggered, this, &ASlashCharacter::ToggleEnemyToLock);

	}

}