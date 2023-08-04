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
		UnSocketWeapon();
	}
	else if (CanArm())
	{
		SocketWeapon(HasWeapon());
	}
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




void ASlashCharacter::SocketWeapon(bool HasWeapon)
{
	ActionState = EActionState::EAS_Equipping;

	//if the player does not already have a stored weapon
	if (!HasWeapon)
	{
		
		AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
		if (OverlappingWeapon)
		{
			EquippedWeapon = OverlappingWeapon;
			EWeaponSize WeaponSize = EquippedWeapon->GetWeaponSize();
			FName SocketName = WeaponSizeToSocketFName(WeaponSize, true);

			EquippedWeapon->Equip(GetMesh(), SocketName);

			CharacterState = WeaponSizeToCharacterState(WeaponSize);

			UE_LOG(LogTemp, Warning, TEXT("NEW weapon block fired"));
		}
		//this is turned off here because there is no associated anim playing at this time
		ActionState = EActionState::EAS_Unoccupied;
	}

	//the player has a stored weapon
	else if (HasWeapon) 
	{

		EWeaponSize WeaponSize = EquippedWeapon->GetWeaponSize();
		FName SocketName = WeaponSizeToSocketFName(WeaponSize, true);
		FName MontageName = WeaponSizeToEquipMontageFName(WeaponSize, true);

		EquippedWeapon->Equip(GetMesh(), SocketName);
		PlayEquipMontage(MontageName);

		CharacterState = WeaponSizeToCharacterState(WeaponSize);

		UE_LOG(LogTemp, Warning, TEXT("HAS weapon block fired"));
			
	}
	
}

void ASlashCharacter::UnSocketWeapon()
{
	ActionState = EActionState::EAS_Equipping;

	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		EquippedWeapon = OverlappingWeapon;
		EWeaponSize WeaponSize = EquippedWeapon->GetWeaponSize();
		FName SocketName = WeaponSizeToSocketFName(WeaponSize, false);
		FName MontageName = WeaponSizeToEquipMontageFName(WeaponSize, false);

		//TODO make a new function on weapon for sheath
		//EquippedWeapon->Equip(GetMesh(), SocketName);
		PlayEquipMontage(MontageName);

		CharacterState = ECharacterState::ECS_Unarmed;
	}
	
}

ECharacterState ASlashCharacter::WeaponSizeToCharacterState(EWeaponSize WeaponSize)
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

FName ASlashCharacter::WeaponSizeToSocketFName(EWeaponSize WeaponSize, bool isEquipping)
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

FName ASlashCharacter::WeaponSizeToEquipMontageFName(EWeaponSize WeaponSize, bool isEquipping)
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



void ASlashCharacter::PlayEquipMontage(FName SectionName)
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
		PlayAttackMontage();
	}

}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		   CharacterState != ECharacterState::ECS_Unarmed;
}




void ASlashCharacter::PlayAttackMontage()
{

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AttackSwordMontage)
	{
		ActionState = EActionState::EAS_Attacking;

		AnimInstance->Montage_Play(AttackSwordMontage);

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
		default:
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, AttackSwordMontage);

	}
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

		
	}

}

