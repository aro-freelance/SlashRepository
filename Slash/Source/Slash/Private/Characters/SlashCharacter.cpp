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
#include "Items/Weapons/Weapon.h"
#include "Items/Weapons/WeaponTypes.h"
#include "Animation/AnimMontage.h"

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

}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SlashCharacterContext, 0);
		}
	}

}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	
	if (ActionState != EActionState::EAS_Unoccupied) return;

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

	UE_LOG(LogTemp, Warning, TEXT("Jump pressed"));

	Super::Jump();

}

void ASlashCharacter::Dodge(const FInputActionValue& Value)
{

	UE_LOG(LogTemp, Warning, TEXT("Dodge pressed"));

}

void ASlashCharacter::Equip(const FInputActionValue& Value)
{
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



bool ASlashCharacter::CanDisarm()
{
	CharacterState = GetCharacterState();

	return ActionState == EActionState::EAS_Unoccupied && 
		CharacterState != ECharacterState::ECS_Unarmed;
}

bool ASlashCharacter::CanArm()
{
	CharacterState = GetCharacterState();

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

		EWeaponSize WeaponSize = EquippedWeapon->GetWeaponSize();

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
	UE_LOG(LogTemp, Warning, TEXT("equip weapon, weapon stored"));
	EWeaponSize WeaponSize = EquippedWeapon->GetWeaponSize();
	FName MontageName = WeaponSizeToEquipMontageFName(WeaponSize, true);
	CharacterState = WeaponSizeToCharacterState(WeaponSize);

	PlayEquipMontage(MontageName);

	//AttachWeapon is called in blueprints when anim notify happens at a certain point in the animation
	
}

void ASlashCharacter::UnEquipAnimation()
{

	EWeaponSize WeaponSize = EquippedWeapon->GetWeaponSize();
	FName MontageName = WeaponSizeToEquipMontageFName(WeaponSize, false);

	CharacterState = ECharacterState::ECS_Unarmed;

	PlayEquipMontage(MontageName);

	//AttachWeapon is called in blueprints when anim notify happens at a certain point in the animation
	
}

ECharacterState ASlashCharacter::WeaponSizeToCharacterState(const EWeaponSize& WeaponSize)
{
	ECharacterState NewCharacterState = ECharacterState();

	//TODO: add cases for other weapon types

	switch (WeaponSize)
	{
	case EWeaponSize::EWS_OneHanded:
		NewCharacterState = ECharacterState::ECS_EquippedOneHanded;
		break;
	case EWeaponSize::EWS_TwoHanded:
		NewCharacterState = ECharacterState::ECS_EquippedTwoHanded;
		break;
	default:
		break;
	}

	return NewCharacterState;
}

FName ASlashCharacter::WeaponSizeToSocketFName(const EWeaponSize& WeaponSize, bool isEquipping)
{
	FName SocketName = FName();

	//TODO: add cases for other weapon types

	switch (WeaponSize)
	{
	case EWeaponSize::EWS_OneHanded: 
		if (isEquipping)
		{
			SocketName = FName("RightHandSocket");
		}
		else
		{
			SocketName = FName("OneHandedSheathSocket");
		}
		break;
	case EWeaponSize::EWS_TwoHanded:
		if (isEquipping)
		{
			SocketName = FName("TwoHandedHammerSocket");
		}
		else
		{
			SocketName = FName("TwoHandedSheathSocket");
		}
		break;
	default:
		break;

	}

	return SocketName;
}

FName ASlashCharacter::WeaponSizeToEquipMontageFName(const EWeaponSize& WeaponSize, bool isEquipping)
{
	FName MontageName = FName();

	//TODO: add cases for other weapon types

	switch (WeaponSize)
	{
	case EWeaponSize::EWS_OneHanded:
		if (isEquipping) 
		{
			MontageName = FName("Equip");
		}
		else 
		{
			MontageName = FName("Unequip");
		}

		break;
	case EWeaponSize::EWS_TwoHanded:
		if (isEquipping)
		{
			MontageName = FName("EquipTwoHanded");
		}
		else
		{
			MontageName = FName("UnequipTwoHanded");
		}

		break;
	default:
		break;

	}

	return MontageName;
}

/*
this is called in blueprints 
when the animation notify happens
from the player hitting the equip button to start the animation
*/
void ASlashCharacter::AttachWeapon(const EWeaponSize& WeaponSize, bool isEquipping)
{
	if (EquippedWeapon)
	{
		//socket the weapon
		FName SocketName = WeaponSizeToSocketFName(WeaponSize, isEquipping);
		EquippedWeapon->AttachMeshToSocket(GetMesh(), SocketName);

		//and then update its itemstate
		if (isEquipping) 
		{
			EquippedWeapon->SetItemState(EItemState::EIS_Held);
		}
		else
		{
			EquippedWeapon->SetItemState(EItemState::EIS_Sheathed);
		}
	}

}




void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && EquipSwordMontage)
	{

		AnimInstance->Montage_Play(EquipSwordMontage);

		AnimInstance->Montage_JumpToSection(SectionName, EquipSwordMontage);

	}
}


void ASlashCharacter::Attack(const FInputActionValue& Value)
{
	
	if (CanAttack()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Can attack"));

		EWeaponSize WeaponSize = EquippedWeapon->GetWeaponSize();

		PlayAttackMontage(WeaponSize);
	}
	else
	{
		FString StateString = BuildStateString();

		UE_LOG(LogTemp, Warning, TEXT("Can't attack because: %s."), *StateString);
	}
}



bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		   CharacterState != ECharacterState::ECS_Unarmed;
}




void ASlashCharacter::PlayAttackMontage(const EWeaponSize& WeaponSize)
{
	//NOTE: when working on animations we can type ~ and then slowmo in UE5 PIE to play slowly

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	//Sword Attacks
	if (AnimInstance && AttackSwordMontage && WeaponSize == EWeaponSize::EWS_OneHanded)
	{
		ActionState = EActionState::EAS_Attacking;

		AnimInstance->Montage_Play(AttackSwordMontage);

		//TODO: fix or remove spin jump
		//case 2 disabled because spin jump animation has issue jumping back after ending.. 
		// and its impact point is odd causing the enemy to get knocked towards player rather than away
		const int32 RandomSelection = FMath::RandRange(0, 1);

		FName SectionName = FName();
		switch (RandomSelection)
		{
		case 0:
			SectionName = FName("Attack_h");
			break;
		case 1:
			SectionName = FName("Attack_v");
			break;
		case 2: 
			SectionName = FName("Attack_spinjump");
			break; 
		default:
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, AttackSwordMontage);

	}

	//Fire Hammer Attacks
	if (AnimInstance && AttackHammerMontage && WeaponSize == EWeaponSize::EWS_TwoHanded)
	{
		ActionState = EActionState::EAS_Attacking;

		AnimInstance->Montage_Play(AttackHammerMontage);


		//TODO: hammer low is disabled because it needs to be extended to hit further from player. right now it is a "pulled punch" type attack and doesn't hit well.
		const int32 RandomSelection = FMath::RandRange(0, 1);
		FName SectionName = FName();
		switch (RandomSelection)
		{
		case 0:
			SectionName = FName("HammerHigh");
			break;
		case 1:
			SectionName = FName("HammerSwing");
			break;
		case 2:
			SectionName = FName("HammerLow");
			break;
		default:
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, AttackHammerMontage);

	}
}

void ASlashCharacter::DropWeapon(const FInputActionValue& Value)
{

	

	UE_LOG(LogTemp, Warning, TEXT("drop weapon pressed"));

	if (EquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("equipped weapon true"));

		if (CanDropWeapon())
		{
			EquippedWeapon->DetachMeshFromSocket();

			if (!EquippedWeapon->IsAttachedTo(this)) 
			{
				EquippedWeapon->SetItemState(EItemState::EIS_Hovering);
				EquippedWeapon = nullptr;
				CharacterState = ECharacterState::ECS_Unarmed;
				ActionState = EActionState::EAS_Unoccupied;
			}
	
		}
	}
}



bool ASlashCharacter::CanDropWeapon()
{
	CharacterState = GetCharacterState();

	if(ActionState == EActionState::EAS_Unoccupied) { UE_LOG(LogTemp, Warning, TEXT("unoccupied is good")); }
	if (CharacterState == ECharacterState::ECS_Unarmed) { UE_LOG(LogTemp, Warning, TEXT("unarmed")); }
	if (CharacterState == ECharacterState::ECS_EquippedOneHanded) { UE_LOG(LogTemp, Warning, TEXT("equipped 1h")); }
	if (CharacterState == ECharacterState::ECS_EquippedTwoHanded) { UE_LOG(LogTemp, Warning, TEXT("equipped 2h")); }
	if (EquippedWeapon->GetItemState() == EItemState::EIS_Held) { UE_LOG(LogTemp, Warning, TEXT("weapon held is good")); }

	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unarmed &&
		EquippedWeapon->GetItemState() == EItemState::EIS_Held;
}



void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

		
	}

}

FString ASlashCharacter::BuildStateString()
{
	FString StateString = FString();

	switch (GetActionState())
	{
	case EActionState::EAS_Unoccupied:
		StateString = "Unoccupied.";
		break;
	case EActionState::EAS_Equipping:
		StateString = "Equipping.";
		break;
	case EActionState::EAS_Attacking:
		StateString = "Attacking.";
		break;
	default:
		break;
	}

	switch (GetCharacterState())
	{
	case ECharacterState::ECS_Unarmed:
		StateString = StateString + " Unarmed";
		break;
	case ECharacterState::ECS_EquippedOneHanded:
		StateString = StateString + " Equipped One Handed";
		break;
	case ECharacterState::ECS_EquippedTwoHanded:
		StateString = StateString + " Equipped Two Handed";
		break;
	default:
		break;
	}

	return StateString;
}

