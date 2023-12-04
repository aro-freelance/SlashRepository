
#include "Characters/BaseCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Components/AttributeComponent.h"


#include "Kismet/GameplayStatics.h"

#include "Items/Weapons/Weapon.h"
#include "Enemy/Enemy.h"
#include "Items/RecoveryPickup.h"


ABaseCharacter::ABaseCharacter()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	
}


void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetWeaponSettings();
	
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	Recover(DeltaTime);

	if (IsBuffed)
	{
		BuffTimer(DeltaTime);
	}

}


/*
this is called in blueprints
when the animation notify happens
from the player hitting the equip button to start the animation
*/
void ABaseCharacter::AttachWeapon(const EWeaponType& WeaponType, bool isEquipping, bool isSecondWeapon)
{

	if (EquippedWeapon)
	{
		//socket the weapon (if isSecondWeapon is a bool that allows for equipping weapons to a second socket if they are the second one)
		FName SocketName = WeaponTypeToSocketFName(WeaponType, isEquipping, isSecondWeapon);
		EquippedWeapon->AttachMeshToSocket(GetMesh(), SocketName);

		SetWeaponSettings();

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

void ABaseCharacter::SetWeaponSettings()
{
	//TODO: set other stuff based on weapon type here. this is called when weapon is attached to character
	if (EquippedWeapon)
	{
		switch (EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_OneHanded:
			AttackMontage = AttackSwordMontage;
			AttackMontageSectionNames = SwordAttackMontageSectionNames;
			HasMeleeWeapon = true;
			break;
		case EWeaponType::EWT_TwoHanded:
			AttackMontage = AttackHammerMontage;
			AttackMontageSectionNames = HammerAttackMontageSectionNames;
			HasMeleeWeapon = true;
			break;
		case EWeaponType::EWT_Rifle:
			AttackMontage = AttackRifleMontage;
			AttackMontageSectionNames = RifleAttackMontageSectionNames;
			HasRangedWeapon = true;
			HasSnipeWeapon = true;
			break;
		case EWeaponType::EWT_Pistol:
			AttackMontage = AttackPistolMontage;
			AttackMontageSectionNames = PistolAttackMontageSectionNames;
			HasRangedWeapon = true;
			break;
		case EWeaponType::EWT_Bow:
			AttackMontage = AttackBowMontage;
			AttackMontageSectionNames = BowAttackMontageSectionNames;
			HasRangedWeapon = true;
			break;
		case EWeaponType::EWT_HandToHand:
			AttackMontage = AttackHandToHandMontage;
			AttackMontageSectionNames = BowAttackMontageSectionNames;
			HasMeleeWeapon = true;
			UsesHandToHand = true;
			break;
		case EWeaponType::EWT_Bite:
			AttackMontage = AttackBiteMontage;
			AttackMontageSectionNames = BiteAttackMontageSectionNames;
			HasMeleeWeapon = true;
			IsAnimal = true;
			break;
		case EWeaponType::EWT_Claw:
			AttackMontage = AttackClawMontage;
			AttackMontageSectionNames = ClawAttackMontageSectionNames;
			HasMeleeWeapon = true;
			IsAnimal = true;
			break;

		}

	}
	/*else
	{
		if (IsAnimal)
		{
			AttackMontage = AttackAnimalMontage;
			AttackMontageSectionNames = AnimalAttackMontageSectionNames;
			HasMeleeWeapon = true;
		} 

		if (UsesHandToHand)
		{
			AttackMontage = AttackHandToHandMontage;
			AttackMontageSectionNames = HandToHandAttackMontageSectionNames;
			HasMeleeWeapon = true;
		}
	
	}*/
}

FName ABaseCharacter::WeaponTypeToSocketFName(const EWeaponType& WeaponType, bool isEquipping, bool isSecondWeapon)
{
	FName SocketName = FName();

	//TODO: add cases for other weapon types

	switch (WeaponType)
	{
	case EWeaponType::EWT_OneHanded:
		if (isEquipping)
		{
			SocketName = FName("RightHandSocket");
		}
		else
		{
			SocketName = FName("OneHandedSheathSocket");
		}
		break;
	case EWeaponType::EWT_TwoHanded:
		if (isEquipping)
		{
			SocketName = FName("TwoHandedHammerSocket");
		}
		else
		{
			SocketName = FName("TwoHandedSheathSocket");
		}
		break;
	case EWeaponType::EWT_Rifle:
		if (isEquipping)
		{
			SocketName = FName("RightHandSocket");
		}
		else
		{
			SocketName = FName("OneHandedSheathSocket");
		}
		break;
	case EWeaponType::EWT_Pistol:
		if (isEquipping)
		{
			SocketName = FName("RightHandSocket");
		}
		else
		{
			SocketName = FName("OneHandedSheathSocket");
		}
		break;
	case EWeaponType::EWT_Bow:
		if (isEquipping)
		{
			SocketName = FName("RightHandSocket");
		}
		else
		{
			SocketName = FName("OneHandedSheathSocket");
		}
		break;
	case EWeaponType::EWT_HandToHand:
		if (isEquipping)
		{
			if (!isSecondWeapon)
			{
				SocketName = FName("RightHandToHandSocket");
			}
			else
			{
				SocketName = FName("LeftHandToHandSocket");
			}
			
		}
		else
		{
			SocketName = FName("OneHandedSheathSocket");
		}
		break;
	case EWeaponType::EWT_Bite:
		if (isEquipping)
		{
			SocketName = FName("MouthSocket");
		}
		else
		{
			SocketName = FName("OneHandedSheathSocket");
		}
		break;
	case EWeaponType::EWT_Claw:
		if (isEquipping)
		{
			if (!isSecondWeapon)
			{
				SocketName = FName("RightClawSocket");
			}
			else
			{
				SocketName = FName("LeftClawSocket");
			}
		}
		else
		{
			SocketName = FName("OneHandedSheathSocket");
		}
		break;

	default:
		break;

	}

	return SocketName;

}



void ABaseCharacter::PlayMontage(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && Montage)
	{

		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);

	}
}


bool ABaseCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unarmed;
}

bool ABaseCharacter::CanSpecialAttack()
{
	bool CanSpecialAttack = false;
	if (Attributes->GetTP() >= SpecialAttackTPCost)
	{
		CanSpecialAttack = true;
	}
	if (IsInfinTP)
	{
		CanSpecialAttack = true;
	}

	return CanSpecialAttack;
}

void ABaseCharacter::SetFollowDistance()
{
	if (EquippedWeapon)
	{
		FollowDistance = EquippedWeapon->GetFollowDistance();
	}
}




FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (!CombatTarget) { return FVector(); }

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();
	FVector DistanceBetweenCharacters = (Location - CombatTargetLocation).GetSafeNormal();
	DistanceBetweenCharacters *= FollowDistance;
	

	return CombatTargetLocation + DistanceBetweenCharacters;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (!CombatTarget) { return FVector(); }
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}


void ABaseCharacter::StartCombat()
{
	if (CombatMode != ECombatMode::ECM_Dead)
	{
		//TODO turn on combat hud

		IsInCombat = true;


		//TODO: Turn on combat music
	}
}

void ABaseCharacter::EndCombat()
{
	//TODO hide combat HUD
	

	//clear last target information
	ClearCombatTarget("EndCombat");
	LastAttacker = nullptr;
	LastHitWeapon = nullptr;
	LastHitImpactPoint = FVector();
	LastHitDirection = FName();
	LastDamageAmount = 0.f;
	IsInCombat = false;

	SetCombatMode(ECombatMode::ECM_OutOfCombat);

	//TODO: Turn off combat music

}

void ABaseCharacter::IncreaseTP()
{
	float TP = Attributes->GetTP();
	float MaxTP = Attributes->GetMaxTP();
	float Amount = FMath::Clamp(TP + (MaxTP * TPGainPercent), 0.0f, MaxTP);
	Attributes->SetTP(Amount);
}

bool ABaseCharacter::HasMultipleWeapons()
{
	bool b = false;

	if (EquippedWeapon)
	{
		if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_HandToHand)
		{
			b = true;
		}
		if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Claw)
		{
			b = true;
		}
	}

	return b;
}

float ABaseCharacter::CalculatePhysicalDamage(float DamageAmount)
{
	float FinalDamageAmount = 0.0f;

	int32 PrecisionRange = LastHitWeapon->GetPrecisionRange();

	if (LastAttacker != nullptr && Attributes)
	{
		UAttributeComponent* SlashCharacterAttributeComponent = LastAttacker->GetAttributes();
		if (SlashCharacterAttributeComponent)
		{
			float AttackerSTR = SlashCharacterAttributeComponent->GetStr();
			float AttackerDEX = SlashCharacterAttributeComponent->GetDex();
			float DefenderVIT = Attributes->GetVit();
			float DefenderAGI = Attributes->GetAgi();

			float PDif = DamageAmount * CompareSTRVIT(AttackerSTR, DefenderVIT);
			int32 Floor = CompareDEXAGI(AttackerDEX, DefenderAGI);
			int32 RandomSelection = Floor + FMath::RandRange(0, PrecisionRange);

			FinalDamageAmount = PDif * ((float)RandomSelection / 100);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("no slash character attribute component"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("no slash character damager causer or Attributes"));
	}

	//the final value is used in TakeDamage to update the HP and set the HPbarwidget
	return FinalDamageAmount;
}

float ABaseCharacter::CalculateMagicalDamage(float DamageAmount)
{
	float FinalDamageAmount = 0.0f;

	int32 PrecisionRange = LastHitWeapon->GetPrecisionRange();

	if (LastAttacker != nullptr && Attributes)
	{
		UAttributeComponent* SlashCharacterAttributeComponent = LastAttacker->GetAttributes();
		if (SlashCharacterAttributeComponent)
		{
			float AttackerINT = SlashCharacterAttributeComponent->GetInt();
			float AttackerCHR = SlashCharacterAttributeComponent->GetChr();
			float DefenderMND = Attributes->GetMnd();
			float DefenderCHR = Attributes->GetChr();

			float PDif = DamageAmount * CompareINTMND(AttackerINT, DefenderMND);
			int32 Floor = CompareCHR(AttackerCHR, DefenderCHR);
			int32 RandomSelection = Floor + FMath::RandRange(0, PrecisionRange);

			FinalDamageAmount = PDif * ((float)RandomSelection / 100);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("no slash character attribute component"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("no slash character damager causer or Attributes"));
	}

	//the final value is used in TakeDamage to update the HP and set the HPbarwidget
	return FinalDamageAmount;
}

float ABaseCharacter::CompareSTRVIT(float AttackerSTR, float DefenderVIT)
{
	float StrVitCalc = 1;

	if (AttackerSTR >= (DefenderVIT * 2))
	{
		StrVitCalc = 2;
	}
	else if ((AttackerSTR * 2) <= DefenderVIT)
	{
		StrVitCalc = 0.5;
	}
	else if (AttackerSTR > DefenderVIT)
	{
		if (AttackerSTR >= (DefenderVIT * 1.5))
		{
			StrVitCalc = 1.5;
		}
		else
		{
			StrVitCalc = 1.25;
		}
	}
	else
	{
		if ((AttackerSTR * 1.5) <= DefenderVIT)
		{
			StrVitCalc = (2 / 3);
		}
		else
		{
			StrVitCalc = (5 / 6);
		}
	}

	if (StrVitCalc <= 0) { StrVitCalc = 0.5; }
	if (StrVitCalc >= 2) { StrVitCalc = 2; }

	return StrVitCalc;
}

int32 ABaseCharacter::CompareDEXAGI(float AttackerDEX, float DefenderAGI)
{
	int32 Floor;

	//if the attacker is more accurate than the enemy's evasion they use a more favorable random damage calculation
	if (AttackerDEX > DefenderAGI)
	{
		UE_LOG(LogTemp, Warning, TEXT("dex > agi"));
		Floor = LastHitWeapon->GetHighAccFloor();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("agi > dex"));
		Floor = LastHitWeapon->GetLowAccFloor();
	}

	return Floor;
}

float ABaseCharacter::CompareINTMND(float AttackerINT, float DefenderMND)
{
	float IntMndCalc = 1;

	if (AttackerINT >= (DefenderMND * 2))
	{
		IntMndCalc = 2;
	}
	else if ((AttackerINT * 2) <= DefenderMND)
	{
		IntMndCalc = 0.5;
	}
	else if (AttackerINT > DefenderMND)
	{
		if (AttackerINT >= (DefenderMND * 1.5))
		{
			IntMndCalc = 1.5;
		}
		else
		{
			IntMndCalc = 1.25;
		}
	}
	else
	{
		if ((AttackerINT * 1.5) <= DefenderMND)
		{
			IntMndCalc = (2 / 3);
		}
		else
		{
			IntMndCalc = (5 / 6);
		}
	}

	if (IntMndCalc <= 0) { IntMndCalc = 0.5; }
	if (IntMndCalc >= 2) { IntMndCalc = 2; }



	return IntMndCalc;
}

int32 ABaseCharacter::CompareCHR(float AttackerCHR, float DefenderCHR)
{
	int32 Floor;

	//if the attacker is more accurate than the enemy's evasion they use a more favorable random damage calculation
	if (AttackerCHR > DefenderCHR)
	{
		UE_LOG(LogTemp, Warning, TEXT("attacker wins chr"));
		Floor = LastHitWeapon->GetHighAccFloor();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("defender wins chr"));
		Floor = LastHitWeapon->GetLowAccFloor();
	}

	return Floor;
}

//TODO: Calculate if the hit is critical
bool ABaseCharacter::CheckCritical(const FVector& ImpactPoint)
{
	return false;
}

FName ABaseCharacter::GetRandomSectionName(TArray<FName> MontageList)
{
	if (MontageList.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("MontageList is Empty"));
		return FName();
	}

	int32 LastIndex = MontageList.Num() - 1;
	const int32 RandomSelection = FMath::RandRange(0, LastIndex);

	return MontageList[RandomSelection];
}

FName ABaseCharacter::CalculateHitReactSectionName(const FVector& ImpactPoint)
{
	//The vector of the direction the actor is facing
	const FVector Forward = GetActorForwardVector();
	//Lower the ImpactPoint to the Actor's Height to create a flat line
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	//AKA ToHit. This is the vector which points from the center of actor location to the point of impact
	const FVector ImpactPointVector = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	//Forward * ImpactPointVector = |Forward||ImpactPointVector| * cos(theta)... 
	// |Forward| = 1, |ImpactPointVector| = 1 therefore Forward * ImpactPointVector = cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ImpactPointVector);
	//Inverse cosine to get theta, and convert it from radians to degrees
	double AngleOfImpact = FMath::RadiansToDegrees(FMath::Acos(CosTheta));

	//this is used to determine the direction of the hit. 
	//If it is negative the hit was from the left, positive the hit was from the right 
	const FVector CrossProduct = FVector::CrossProduct(Forward, ImpactPointVector);
	if (CrossProduct.Z < 0)
	{
		//if crossproduct is negative, inverse the angle of impact
		AngleOfImpact = AngleOfImpact * -1.f;
	}

	FName SectionName = FName("FromBack");

	if (AngleOfImpact >= -45.f && AngleOfImpact < 45.f)
	{
		SectionName = FName("FromFront");
	}
	else if (AngleOfImpact >= -135.f && AngleOfImpact < -45.f)
	{
		SectionName = FName("FromLeft");
	}
	else if (AngleOfImpact >= 45 && AngleOfImpact < 135)
	{
		SectionName = FName("FromRight");
	}

	return SectionName;
}

FName ABaseCharacter::CalculateDeathMontageSectionName()
{
	FName SectionName = "DeathSweep";
	DeathPose = EDeathPose::EDP_Sweep;

	if (LastHitDirection == "FromBack")
	{
		SectionName = "DeathForward";
		DeathPose = EDeathPose::EDP_FallForward;
	}
	else if (LastHitDirection == "FromRight")
	{
		const int32 RandomSelection = FMath::RandRange(0, 2);
		switch (RandomSelection)
		{
		case 0:
			SectionName = "DeathForward";
			DeathPose = EDeathPose::EDP_FallForward;
			break;
		case 1:
			SectionName = "DeathSweep";
			DeathPose = EDeathPose::EDP_Sweep;
			break;
		case 2:
			SectionName = "DeathBack";
			DeathPose = EDeathPose::EDP_FallBack;
			break;
		default:
			break;
		}
	}
	else if (LastHitDirection == "FromLeft")
	{
		const int32 RandomSelection = FMath::RandRange(0, 3);
		switch (RandomSelection)
		{
		case 0:
			SectionName = "DeathForward";
			DeathPose = EDeathPose::EDP_FallForward;
			break;
		case 1:
			SectionName = "DeathSweep";
			DeathPose = EDeathPose::EDP_Sweep;
			break;
		case 2:
			SectionName = "DeathBack";
			DeathPose = EDeathPose::EDP_FallBack;
			break;
		case 3:
			SectionName = "DeathShoulder";
			DeathPose = EDeathPose::EDP_Shoulder;
			break;
		default:
			break;
		}
	}
	else
	{
		//"FromFront"
		const int32 RandomSelection = FMath::RandRange(0, 1);
		switch (RandomSelection)
		{
		case 0:
			SectionName = "DeathSweep";
			DeathPose = EDeathPose::EDP_Sweep;
			break;
		case 1:
			SectionName = "DeathBack";
			DeathPose = EDeathPose::EDP_FallBack;
			break;
		default:
			break;
		}
	}

	return SectionName;
}


void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, ACharacter* DamageDealer, AWeapon* Weapon)
{
	if (CombatMode == ECombatMode::ECM_Dead) { return; }

	ActionState = EActionState::EAS_HitReacting;

	ABaseCharacter* Attacker = Cast<ABaseCharacter>(DamageDealer);
	if (Attacker) 
	{
		//tell the attacker they scored a hit
		Attacker->ProcessHitTarget(this);

		if (!CombatTarget) 
		{ 
			SetCombatTarget(Attacker, "GetHit");
		}

		//Store Information about the Attacker and Weapon of Attacker
		LastAttacker = Attacker;
		LastHitWeapon = Weapon;
		LastHitImpactPoint = ImpactPoint;
		LastHitDirection = CalculateHitReactSectionName(DamageDealer->GetActorLocation());

		if(!IsInCombat){ StartCombat(); }

		//TODO: playtest where the best place to add this multiplier is... 
		// this is increasing the weapon damage currently, 
		// the other option would be to change it in the CalculatePhys or Magic functions after other mods
		// doing it here should result in a stronger buff than further down the line
		float PowerMultipler = Attacker->GetPowerMultiplier();
		float MultiplierWeaponDamage = PowerMultipler * Weapon->GetWeaponDamage();
		
		//Deal Damage
		UGameplayStatics::ApplyDamage(
			this,
			MultiplierWeaponDamage,
			DamageDealer->GetController(),
			Weapon,
			UDamageType::StaticClass()
		);

		//React to the hit with Death or ELSE a normal hit.
		if (!Attributes->IsAlive()){ Death(); }
		else
		{
			HP = Attributes->GetHP();
			MaxHP = Attributes->GetMaxHP();

			//Update TP
			IncreaseTP();

			PlayMontage(HitReactMontage, LastHitDirection);

			//end attack states
			AbortAttack();

			PlaySoundLocal(HitSound, ImpactPoint);

			if (HitParticles && GetWorld())
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					HitParticles,
					ImpactPoint,
					HitParticleRotation,
					HitParticleScale
				);
			}
		}

	}

}

void ABaseCharacter::FinishGetHit()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ABaseCharacter::PlaySoundLocal(USoundBase* Sound, const FVector& Location)
{
	UGameplayStatics::PlaySoundAtLocation(
		this,
		Sound,
		Location
	);
}

//called in Tick if IsBuffed
void ABaseCharacter::BuffTimer(float DeltaTime)
{
	float BuffTickTimerDeltaTime = BuffTickTimer * DeltaTime;
	float BuffDurationDeltaTime = LastBuffReceivedDuration * DeltaTime;

	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(this);
	if (SlashCharacter)
	{
		float PopupDisplayTime = SlashCharacter->GetPopupDisplayTime();
		float PopUpDurationDeltaTime = PopupDisplayTime * DeltaTime;

		//when the popup display timer is over, turn it off
		if (BuffTickTimerDeltaTime >= PopUpDurationDeltaTime)
		{
			SlashCharacter->ClearCenterPopupText();
		}
	}
	


	//if the time elapsed is greater than the buff duration, turn off the buffs 
	// (TODO: if we are keeping multiple buffs keep track of each and deactivate seperately.)
	if (BuffTickTimerDeltaTime >= BuffDurationDeltaTime)
	{
		if (SlashCharacter)
		{
			SlashCharacter->ClearCenterPopupText();
		}

		IsInvincible = false;
		IsInfinMP = false; //TODO: when mp is added implement this the same way as tp and stam
		IsInfinTP = false;
		IsInfinStam = false;
		XPMultiplier = 1.f;
		GoldMultiplier = 1.f;
		SpeedMultiplier = 1.f;
		PowerMultiplier = 1.f;

		//and turn this timer off until another buff is picked up.
		IsBuffed = false;

		//TODO: if visual effect is added turn it OFF here

	}

	BuffTickTimer += 1;

}


float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (CombatMode == ECombatMode::ECM_Dead) { return 0.f; }

	float FinalDamageAmount = 0.0f;

	if (Attributes && LastHitWeapon) 
	{
		//get the weapon's magic damage to physical damage ratio
		float PercentMagicDamage = LastHitWeapon->GetPercentMagicDamage();

		//handle critical hits
		bool isCritical = CheckCritical(LastHitImpactPoint);
		if (isCritical) { DamageAmount = (DamageAmount * LastHitWeapon->GetCriticalMultiplier()); }

		//Calculate Damage to Deal to HP
		if (PercentMagicDamage == 1)
		{
			FinalDamageAmount = CalculateMagicalDamage(DamageAmount);
			UE_LOG(LogTemp, Warning, TEXT("magic damage. %f"), FinalDamageAmount);
		}
		else if (PercentMagicDamage == 0)
		{
			FinalDamageAmount = CalculatePhysicalDamage(DamageAmount);
			UE_LOG(LogTemp, Warning, TEXT("physical damage. %f"), FinalDamageAmount);
		}
		else
		{
			float MagicDamageAmount = DamageAmount * PercentMagicDamage;
			float PhysicalDamageAmount = DamageAmount * (1 - PercentMagicDamage);

			FinalDamageAmount = CalculateMagicalDamage(MagicDamageAmount) + CalculatePhysicalDamage(PhysicalDamageAmount);
			UE_LOG(LogTemp, Warning, TEXT("hybrid damage. Final: %f. Magic: %f. Physical: %f."), FinalDamageAmount, MagicDamageAmount, PhysicalDamageAmount);
		}

		//check for invincible buff
		if (IsInvincible)
		{
			FinalDamageAmount = 0;
		}

		//store the damage amount
		LastDamageAmount = FinalDamageAmount;

		//Log Message for Damage Dealt
		if (GEngine)
		{
			int32 FinalDamageAmountRounded = FMath::RoundToInt(LastDamageAmount);
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, FString::Printf(TEXT("%s was hit by %s using %s for %i damage"), *EnemyName, *CharacterWhoDamagedEnemy->GetName(), *DamagerWeaponName, FinalDamageAmountRounded));

			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("%s was hit by %s for %i damage"), *Name, *LastHitWeapon->WeaponName, FinalDamageAmountRounded));

		}

		UE_LOG(LogTemp, Warning, TEXT("final damage: %f"), FinalDamageAmount);

		//Update HP Amount
		Attributes->ReceiveDamage(FinalDamageAmount);

		//Update Health Bar and other HUD elements
		UpdateCombatHUD();

	}

	return FinalDamageAmount;
}

void ABaseCharacter::Death()
{

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Log Message for Defeat
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s defeated %s"), *LastAttacker->GetName(), *Name));
	}

	//TODO
	//UE_LOG(LogTemp, Warning, TEXT("%s defeated %s"), *CombatTarget->GetName(), *EnemyName);

	PlayMontage(DeathMontage, CalculateDeathMontageSectionName());

	PlaySoundLocal(DeathSound, GetActorLocation());

	//Turn off this Enemy's HUD info, combat behaviors
	//AbortAttack();
	//EndCombat();
	SetCombatMode(ECombatMode::ECM_Dead);
	ClearCombatTarget("Death of Self");
	if (LastAttacker)
	{
		LastAttacker->ClearCombatTarget("Death of Target");
	}


	//turn off the collision capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Change this Character to not be a target of attacks
	Tags.Add(FName("Dead"));

	//play death HUD
	//TODO

}

void ABaseCharacter::SetCombatMode(ECombatMode NewCombatMode)
{
	FString CombatModeString = UEnum::GetDisplayValueAsText(NewCombatMode).ToString();
	FString FullName = GetName();

	if (NewCombatMode == CombatMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s : tried to set CombatMode to %s but prevented because DUPLICATE CALL "), *FullName, *CombatModeString);
	}

	//if not dead, set new combatmode
	if (CombatMode != ECombatMode::ECM_Dead) 
	{
		UE_LOG(LogTemp, Warning, TEXT("%s : CombatMode set to %s"), *FullName, *CombatModeString);
		CombatMode = NewCombatMode;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: SetCombatMode Called for %s but prevented because DEAD."), *FullName, *CombatModeString);
	}
	
}

void ABaseCharacter::SetCombatTarget(ABaseCharacter* Target, FString NameOfFunctionCallingThis)
{
	UE_LOG(LogTemp, Warning, TEXT("%s : %s setting new combat target: %s"), 
		*NameOfFunctionCallingThis, *GetName(), *Target->GetName());

	CombatTarget = Target;
}

void ABaseCharacter::ClearCombatTarget(FString NameOfFunctionCallingThis)
{
	UE_LOG(LogTemp, Warning, TEXT("%s : %s setting new combat target: nullptr"),
		*NameOfFunctionCallingThis, *GetName());

	CombatTarget = nullptr;
}



void ABaseCharacter::MeleeAttack()
{

	UE_LOG(LogTemp, Warning, TEXT("MeleeAttack Method called by %s"), *GetName());

	if (!EquippedWeapon && !IsAnimal && !UsesHandToHand)
	{
		UE_LOG(LogTemp, Warning, TEXT("no viable weapon"));
		return;
	}

	//TODO remove when animal attack functional
	if (IsAnimal)
	{
		UE_LOG(LogTemp, Warning, TEXT("is animal"));
	}

	if (UsesHandToHand)
	{
		UE_LOG(LogTemp, Warning, TEXT("uses h2h"));
	}


	SetCombatMode(ECombatMode::ECM_MeleeAttacking);
	ActionState = EActionState::EAS_Attacking;

	//do the attack
	PlayMontage(AttackMontage, GetRandomSectionName(AttackMontageSectionNames));

	if (EquippedWeapon->CanFire())
	{
		EquippedWeapon->Fire();
	}

	bool AttackHitTarget = true;
	//TODO: update this based on the attack hitting or not
	if (AttackHitTarget)
	{
		IncreaseTP();
	}

	//TODO: turn chasing back on when attack anim is done?

}



void ABaseCharacter::RangedAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("RangedAttack Method"));

	if (!EquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("no equipped weapon"));
		return;
	}

	SetCombatMode(ECombatMode::ECM_RangeAttacking);
	ActionState = EActionState::EAS_Attacking;

	//do the attack
	PlayMontage(AttackMontage, GetRandomSectionName(AttackMontageSectionNames));

	if (EquippedWeapon->CanFire())
	{
		EquippedWeapon->Fire();
	}

	//TODO: we do want attack montage here but we don't want force in the blueprint. 
	// also maybe instead of montage we just want to hold the gun in front all the time instead and not play montage
	// to make it play smoother

	bool AttackHitTarget = true;
	//TODO: update this based on the attack hitting or not
	if (AttackHitTarget)
	{
		IncreaseTP();
	}

	//TODO: turn chasing back on when attack anim is done?

}

void ABaseCharacter::SnipeAttack()
{

	UE_LOG(LogTemp, Warning, TEXT("SnipeAttack Method"));

	if (!EquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("no equipped weapon"));
		return;
	}

	SetCombatMode(ECombatMode::ECM_SnipeAttacking);
	ActionState = EActionState::EAS_Attacking;

	//do the attack
	PlayMontage(AttackMontage, GetRandomSectionName(AttackMontageSectionNames));

	if (EquippedWeapon->CanFire())
	{
		EquippedWeapon->Fire();
	}

	bool AttackHitTarget = true;
	//TODO: update this based on the attack hitting or not
	if (AttackHitTarget)
	{
		IncreaseTP();
	}

	//TODO: turn chasing back on when attack anim is done?
}

void ABaseCharacter::SpecialAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("SpecialAttack Method"));

	if (!EquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("no equipped weapon"));
		return;
	}

	if (CanSpecialAttack())
	{
		if (!IsInfinTP) 
		{
			//use the TP
			Attributes->SetTP(Attributes->GetTP() - SpecialAttackTPCost);
		}
		
		SetCombatMode(ECombatMode::ECM_SpecialAttacking);
		ActionState = EActionState::EAS_Attacking;

		//do the attack
		PlayMontage(AttackMontage, GetRandomSectionName(AttackMontageSectionNames));

		//PlayAttackMontage(WeaponType);
		if (EquippedWeapon->CanFire())
		{
			EquippedWeapon->Fire();
		}

		bool AttackHitTarget = true;
		//TODO: update this based on the attack hitting or not
		if (AttackHitTarget)
		{
			IncreaseTP();
		}
	}
	else
	{
		//TODO: tell player they don't have enough TP
		UE_LOG(LogTemp, Warning, TEXT("Not enough TP to use special move"));
	}

	

	//TODO: turn chasing back on when attack anim is done?
}

void ABaseCharacter::Defend()
{
	UE_LOG(LogTemp, Warning, TEXT("Defend Method"));

	SetCombatMode(ECombatMode::ECM_Defending);

	//TODO: execute the defend logic

	//TODO: turn chasing back on when defend anim is done?
}

void ABaseCharacter::Dodge()
{
	UE_LOG(LogTemp, Warning, TEXT("Dodge Method"));

	SetCombatMode(ECombatMode::ECM_Dodging);

	//TODO: execute the dodge logic
	PlayMontage(DodgeMontage, GetRandomSectionName(DodgeMontageSectionNames));

	//TODO: turn chasing back on when dodge anim is done?
}

void ABaseCharacter::Hide()
{
	UE_LOG(LogTemp, Warning, TEXT("Hide Method"));

	SetCombatMode(ECombatMode::ECM_Hiding);

	//TODO: execute the hide logic

	//TODO: make a check to see if can attack from here, flee or stay here
}


bool ABaseCharacter::CanRecover()
{
	bool b = true;

	if (CombatMode == ECombatMode::ECM_Dead)
	{
		b = false;
	}
	//TODO: add other conditions that would prevent recovery. 
	// Potentially make different functions to check canregen, canmanaregen etc to be more specific
	//  and call them separately in recover or split recover to multiple functions.

	return b;
}

bool ABaseCharacter::CanSee()
{
	bool b = true;

	if (CombatMode == ECombatMode::ECM_Dead)
	{
		b = false;
	}
	//TODO: add other conditions that would prevent seeing. 

	return b;
}

//TODO: change this to an interface to give different functionality to players and enemies
//when out of combat, without full hp/mp increase hp/mp per tick
void ABaseCharacter::Recover(float DeltaTime)
{
	if(CanRecover())
	{
		float RegenTickTimerDeltaTime = RegenTickTimer * DeltaTime;
		float RegenTickLengthDeltaTime = RegenTickLength * DeltaTime;

		if (RegenTickTimerDeltaTime >= RegenTickLengthDeltaTime)
		{
			AEnemy* Enemy = Cast<AEnemy>(this);
			if (Enemy)
			{
				if (CombatMode == ECombatMode::ECM_OutOfCombat)
				{
					Regen();
				}
			}


			RegenStamina();

			//TODO: other recovery aspects? MP? TP?

			RegenTickTimer = 0.0f;

		}

		RegenTickTimer += 1;
	}

}

void ABaseCharacter::RegenStamina()
{
	if (Attributes)
	{
		float CurrentStamina = Attributes->GetStamina();
		float LocalMaxStamina = Attributes->GetMaxStamina();
		float StaminaRegenPercent = Attributes->GetStaminaRegenPercent();

		if (CurrentStamina < LocalMaxStamina)
		{

			float Amount = FMath::Clamp(CurrentStamina + (LocalMaxStamina * StaminaRegenPercent), 0.0f, LocalMaxStamina);

			//change the stat amount
			Attributes->SetStamina(Amount);

			//update the healthbar
			UpdateCombatHUD();
		}

	}
}

void ABaseCharacter::Regen()
{
	//if less than max hp, recover hp by regen percent
	if (Attributes)
	{
		float CurrentHP = Attributes->GetHP();
		float LocalMaxHP = Attributes->GetMaxHP();
		float CurrentRegenPercent = Attributes->GetRegenPercent();

		if (CurrentHP < LocalMaxHP)
		{

			float Amount = FMath::Clamp(CurrentHP + (LocalMaxHP * CurrentRegenPercent), 0.0f, LocalMaxHP);

			//change the stat amount
			Attributes->SetHP(Amount);

			//Update the Blueprint Accessable Stats
			HP = Attributes->GetHP();
			MaxHP = Attributes->GetMaxHP();
			RegenPercent = Attributes->GetRegenPercent();

			//update the healthbar
			UpdateCombatHUD();

			//if full hp, end regen
			if (Attributes->GetHP() >= Attributes->GetMaxHP())
			{
				UE_LOG(LogTemp, Warning, TEXT("regen off"));
				IsRegening = false;

				if (HealthBarWidget)
				{
					HealthBarWidget->SetVisibility(false);
				}
			}
		}

	}
}

void ABaseCharacter::AbortAttack()
{
	SetCombatMode(ECombatMode::ECM_ReadyInCombat);
	ActionState = EActionState::EAS_Unoccupied;
	if(EquippedWeapon){ EquippedWeapon->SetWeaponCollisionState(EWeaponCollisionState::EWS_CollisionOff); }
	
}

void ABaseCharacter::SetReadyInCombat()
{
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(this);
	AEnemy* Enemy = Cast<AEnemy>(this);

	if (SlashCharacter)
	{
		SetCombatMode(ECombatMode::ECM_ReadyInCombat);
	}
	if (Enemy)
	{
		SetCombatMode(ECombatMode::ECM_Chasing);
	}
}


void ABaseCharacter::ProcessHitTarget(AActor* TargetHit)
{
	//if the target hit is a character,
	ABaseCharacter* CharacterHit = Cast<ABaseCharacter>(TargetHit);
	if (CharacterHit)
	{
		if (!CombatTarget) 
		{
			SetCombatTarget(CharacterHit, "ProcessHitTarget");
		}
		
		//if you defeated target
		if (CharacterHit->GetAttributes()->GetHP() <= 0)
		{
			DefeatTargetCharacter(CharacterHit);
		}
	}

	//TODO implement more reactions to hitting a character and/or item
	
}

void ABaseCharacter::DefeatTargetCharacter(ABaseCharacter* CharacterHit)
{
	
	
	ASlashCharacter* ThisSlashCharacter = Cast<ASlashCharacter>(this);
	if (ThisSlashCharacter) 
	{
		//TODO: player rewards for victory
		UE_LOG(LogTemp, Warning, TEXT("You defeated %s."), *CharacterHit->GetName());

		//clear the dead target from CombatTarget
		ClearCombatTarget("DefeatTargetCharacter as player");

	}
	else
	{
		//handle enemy defeating player
		UE_LOG(LogTemp, Warning, TEXT("You were defeated by %s."), *this->GetName());
		
		//end current attack
		AbortAttack();

		//TODO: change this to...
		// CombatTarget = nullptr
		// if no other targets in range 
		// end combat
		EndCombat();
	}
	
	
	

	

		
}

void ABaseCharacter::UnequipWeapon()
{
	EquippedWeapon->DetachMeshFromSocket();

	EquippedWeapon = nullptr;
	CharacterState = ECharacterState::ECS_Unarmed;
	ActionState = EActionState::EAS_Unoccupied;

	//TODO: set height from ground. when the weapon is dropping from dead enemy it is fairly high.
}

void ABaseCharacter::UpdateCombatHUD()
{
	//this is handled in the overrides in Enemy and SlashCharacter... 
	// TODO: if we want shared functionality put it here.
}

//Return the user friendly name of this character
FString ABaseCharacter::GetName()
{
	return Name;
}






