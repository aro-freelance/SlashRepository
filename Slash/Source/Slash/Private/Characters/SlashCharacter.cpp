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
	//Add later
	//if (ActionState != EActionState::EAS_Unoccupied) return;

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

	CharacterState = GetCharacterState();

	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);

	FString StateString = *UEnum::GetValueAsString(CharacterState);

	//UE_LOG(LogTemp, Warning, TEXT("Equip pressed %s"), StateString);

	if (CharacterState != ECharacterState::ECS_Unarmed)
	{
		//unequip weapon in socket
		if (OverlappingWeapon)
		{
			EWeaponSize WeaponSize = OverlappingWeapon->GetWeaponSize();
			if (WeaponSize == EWeaponSize::ECS_TwoHanded)
			{
				SetCharacterState(ECharacterState::ECS_Unarmed);
				//remove from "TwoHandedHammerSocket"
				UE_LOG(LogTemp, Warning, TEXT("two hand unequip"));
			}
			else
			{
				SetCharacterState(ECharacterState::ECS_Unarmed);
				//remove from "RightHandSocket"
				UE_LOG(LogTemp, Warning, TEXT("one hand unequip"));
			}

		}
		
	}
	else
	{

		if (OverlappingWeapon)
		{
			EWeaponSize WeaponSize = OverlappingWeapon->GetWeaponSize();
			if (WeaponSize == EWeaponSize::ECS_TwoHanded)
			{
				SetCharacterState(ECharacterState::ECS_EquippedTwoHanded);
				OverlappingWeapon->Equip(GetMesh(), FName("TwoHandedHammerSocket"));
				UE_LOG(LogTemp, Warning, TEXT("two hand equip"));
			}
			else
			{
				SetCharacterState(ECharacterState::ECS_EquippedOneHanded);
				OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"));
				UE_LOG(LogTemp, Warning, TEXT("one hand equip"));
			}

		}
	}

	

}

void ASlashCharacter::Attack(const FInputActionValue& Value)
{

	UE_LOG(LogTemp, Warning, TEXT("Attack pressed"));

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AttackSwordMontage)
	{

		AnimInstance->Montage_Play(AttackSwordMontage);

		int32 RandomSelection = FMath::RandRange(0, 1);
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

