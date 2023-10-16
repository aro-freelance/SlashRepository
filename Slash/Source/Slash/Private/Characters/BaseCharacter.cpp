
#include "Characters/BaseCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Components/AttributeComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Items/Weapons/Weapon.h"





ABaseCharacter::ABaseCharacter()
{
 	
	PrimaryActorTick.bCanEverTick = true;

}


void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


/*
this is called in blueprints
when the animation notify happens
from the player hitting the equip button to start the animation
*/
void ABaseCharacter::AttachWeapon(const EWeaponType& WeaponType, bool isEquipping)
{
	if (EquippedWeapon)
	{
		//socket the weapon
		FName SocketName = WeaponTypeToSocketFName(WeaponType, isEquipping);
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

FName ABaseCharacter::WeaponTypeToSocketFName(const EWeaponType& WeaponType, bool isEquipping)
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
	default:
		break;

	}

	return SocketName;

}

void ABaseCharacter::PlayAttackMontage(const EWeaponType& WeaponSize)
{

	//NOTE: when working on animations we can type ~ and then slowmo in UE5 PIE to play slowly

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	//Sword Attacks
	if (AnimInstance && AttackSwordMontage && WeaponSize == EWeaponType::EWT_OneHanded)
	{
		ActionState = EActionState::EAS_Attacking;

		AnimInstance->Montage_Play(AttackSwordMontage);

		//TODO: fix or remove spin jump (renamed swordattack3) @Yelsa Fix sword attack 1
		//case 2 disabled because spin jump animation has issue jumping back after ending.. 
		// and its impact point is odd causing the enemy to get knocked towards player rather than away
		const int32 RandomSelection = FMath::RandRange(0, 1);

		FName SectionName = FName();
		switch (RandomSelection)
		{
		case 0:
			SectionName = FName("SwordAttack1");
			break;
		case 1:
			SectionName = FName("SwordAttack2");
			break;
		case 2:
			SectionName = FName("SwordAttack3");
		default:
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, AttackSwordMontage);

	}

	//Fire Hammer Attacks
	if (AnimInstance && AttackHammerMontage && WeaponSize == EWeaponType::EWT_TwoHanded)
	{
		ActionState = EActionState::EAS_Attacking;

		AnimInstance->Montage_Play(AttackHammerMontage);


		//TODO: hammer low (renamed hammerattack0) is disabled because it needs to be extended to hit further from player. right now it is a "pulled punch" type attack and doesn't hit well.
		const int32 RandomSelection = FMath::RandRange(0, 1);
		FName SectionName = FName();
		switch (RandomSelection)
		{
		case 0:
			SectionName = FName("HammerAttack1");
			break;
		case 1:
			SectionName = FName("HammerAttack2");
			break;
		case 2:
			SectionName = FName("HammerAttack0");
			break;
		default:
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, AttackHammerMontage);

	}

	//Rifle Attacks
	if (AnimInstance && AttackHammerMontage && WeaponSize == EWeaponType::EWT_Rifle)
	{
		ActionState = EActionState::EAS_Attacking;

		AnimInstance->Montage_Play(AttackRifleMontage);


		const int32 RandomSelection = FMath::RandRange(0, 1);
		FName SectionName = FName();
		switch (RandomSelection)
		{
		case 0:
			SectionName = FName("RifleAttack1");
			break;
		case 1:
			SectionName = FName("RifleAttack2");
			break;
		default:
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, AttackRifleMontage);

	}

	//Pistol Attacks
	if (AnimInstance && AttackHammerMontage && WeaponSize == EWeaponType::EWT_Pistol)
	{
		ActionState = EActionState::EAS_Attacking;

		AnimInstance->Montage_Play(AttackPistolMontage);


		const int32 RandomSelection = FMath::RandRange(0, 1);
		FName SectionName = FName();
		switch (RandomSelection)
		{
		case 0:
			SectionName = FName("PistolAttack1");
			break;
		case 1:
			SectionName = FName("PistolAttack2");
			break;
		default:
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, AttackPistolMontage);

	}

	//Bow Attacks
	if (AnimInstance && AttackHammerMontage && WeaponSize == EWeaponType::EWT_Bow)
	{
		ActionState = EActionState::EAS_Attacking;

		AnimInstance->Montage_Play(AttackBowMontage);


		//TODO: add more bow attacks. NOTE THAT THEY ARE THE SAME ATM 
		const int32 RandomSelection = FMath::RandRange(0, 1);
		FName SectionName = FName();
		switch (RandomSelection)
		{
		case 0:
			SectionName = FName("BowAttack1");
			break;
		case 1:
			//TODO: get more bow attacks?
			SectionName = FName("BowAttack1");
			break;
		default:
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, AttackBowMontage);

	}

}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HitReactMontage)
	{

		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);

	}
}

void ABaseCharacter::PlayDeathMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && DeathMontage)
	{

		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);

	}
}

bool ABaseCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unarmed;
}

FString ABaseCharacter::BuildCharacterStateString()
{
	FString StateString = FString();

	switch (ActionState)
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

	switch (CharacterState)
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

void ABaseCharacter::StartCombat()
{
	
	//TODO turn on combat hud

	IsInCombat = true;
	

	//TODO: Turn on combat music

	/*
	* DO THIS IN ENEMY OVERRIDE OF THIS FUNCTION
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
	CombatMode = ECombatMode::ECM_Chasing;

	*/

}

void ABaseCharacter::EndCombat()
{
	//TODO hide combat HUD
	

	//clear last target information
	CombatTarget = nullptr;
	LastHitWeapon = nullptr;
	LastHitImpactPoint = FVector();
	LastHitDirection = FName();
	LastDamageAmount = 0.f;
	IsInCombat = false;
	

	//TODO: Turn off combat music

	/* DO THIS IN ENEMY OVERRIDE OF THIS FUNCTION
	
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
	CombatMode = ECombatMode::ECM_OutOfCombat;
	
	*/

}

void ABaseCharacter::IncreaseTP()
{
	float TP = Attributes->GetTP();
	float MaxTP = Attributes->GetMaxTP();
	float Amount = FMath::Clamp(TP + (MaxTP * TPGainPercent), 0.0f, MaxTP);
	Attributes->SetTP(Amount);
}

float ABaseCharacter::CalculatePhysicalDamage(float DamageAmount)
{
	float FinalDamageAmount = 0.0f;

	int32 PrecisionRange = LastHitWeapon->GetPrecisionRange();

	if (CombatTarget != nullptr && Attributes)
	{
		UAttributeComponent* SlashCharacterAttributeComponent = CombatTarget->GetAttributes();
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

	if (CombatTarget != nullptr && Attributes)
	{
		UAttributeComponent* SlashCharacterAttributeComponent = CombatTarget->GetAttributes();
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
	UE_LOG(LogTemp, Warning, TEXT("Get Hit called by %s."), *GetName());

	//TODO: fix this for player. 
	// one of the bugs we have atm  is that when player is hit they can no longer move or respond. figure out why and fix.

	ABaseCharacter* Attacker = Cast<ABaseCharacter>(DamageDealer);
	if (Attacker) 
	{

		//Store Information about the Attacker and Weapon of Attacker
		CombatTarget = Attacker;
		LastHitWeapon = Weapon;
		LastHitImpactPoint = ImpactPoint;
		LastHitDirection = CalculateHitReactSectionName(ImpactPoint);

		if(!IsInCombat){ StartCombat(); }
		
		//Deal Damage
		UGameplayStatics::ApplyDamage(
			this,
			Weapon->GetWeaponDamage(),
			DamageDealer->GetController(),
			Weapon,
			UDamageType::StaticClass()
		);

		//React to the hit with Death or ELSE a normal hit.
		if (!Attributes->IsAlive())
		{
			Death();
		}
		else
		{
			HP = Attributes->GetHP();
			MaxHP = Attributes->GetMaxHP();

			//Update TP
			IncreaseTP();

			PlayHitReactMontage(LastHitDirection);

			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					HitSound,
					ImpactPoint
				);
			}

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
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DamageDealer cast to BaseCharacter failed."));
	}


}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = 0.0f;

	if (!Attributes) { UE_LOG(LogTemp, Warning, TEXT("Take Damage: !Attributes")); }
	//if (!HealthBarWidget) { UE_LOG(LogTemp, Warning, TEXT("Take Damage: !HealthBarWidget")); }
	if (!LastHitWeapon) { UE_LOG(LogTemp, Warning, TEXT("Take Damage: !LastHitWeapon")); }

	if (Attributes && LastHitWeapon) //&& HealthBarWidget 
	{
		UE_LOG(LogTemp, Warning, TEXT("Take Damage called by %s"), *GetName());

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
	//Log Message for Defeat
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s defeated %s"), *CombatTarget->GetName(), *Name));
	}

	//TODO
	//UE_LOG(LogTemp, Warning, TEXT("%s defeated %s"), *CombatTarget->GetName(), *EnemyName);

	FName SectionName = CalculateDeathMontageSectionName();
	PlayDeathMontage(SectionName);

	//Turn off this Enemy's HUD info, combat behaviors
	EndCombat();


}


void ABaseCharacter::MeleeAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("MeleeAttack Method called by %s"), *GetName());

	if (!EquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("no equipped weapon"));
		return;
	}

	ActionState = EActionState::EAS_Attacking;

	//TODO: do the attack
	EWeaponType WeaponType = EquippedWeapon->GetWeaponType();
	PlayAttackMontage(WeaponType);
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

	ActionState = EActionState::EAS_Attacking;

	//TODO: do the attack
	EWeaponType WeaponType = EquippedWeapon->GetWeaponType();
	PlayAttackMontage(WeaponType);
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

	ActionState = EActionState::EAS_Attacking;

	//TODO: do the attack
	EWeaponType WeaponType = EquippedWeapon->GetWeaponType();
	PlayAttackMontage(WeaponType);
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

	//use the TP
	Attributes->SetTP(Attributes->GetTP() - SpecialAttackTPCost);

	ActionState = EActionState::EAS_Attacking;

	//TODO: do the attack
	EWeaponType WeaponType = EquippedWeapon->GetWeaponType();
	PlayAttackMontage(WeaponType);
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

void ABaseCharacter::Defend()
{
	UE_LOG(LogTemp, Warning, TEXT("Defend Method"));

	//TODO: execute the defend logic

	//TODO: turn chasing back on when defend anim is done?
}

void ABaseCharacter::Dodge()
{
	UE_LOG(LogTemp, Warning, TEXT("Dodge Method"));

	//TODO: execute the dodge logic

	//TODO: turn chasing back on when dodge anim is done?
}

void ABaseCharacter::Hide()
{
	UE_LOG(LogTemp, Warning, TEXT("Hide Method"));

	//TODO: execute the hide logic

	//TODO: make a check to see if can attack from here, flee or stay here
}


//when out of combat, without full hp/mp increase hp/mp per tick
void ABaseCharacter::Recover(float DeltaTime)
{

	float RegenTickTimerDeltaTime = RegenTickTimer * DeltaTime;
	float RegenTickLengthDeltaTime = RegenTickLength * DeltaTime;

	if (RegenTickTimerDeltaTime >= RegenTickLengthDeltaTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("recover before: %f"), Attributes->GetHP());

		//if less than max hp, recover hp by regen percent
		if (Attributes->GetHP() && Attributes->GetMaxHP() && Attributes->GetRegenPercent())
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

			UE_LOG(LogTemp, Warning, TEXT("recover after: %f"), Attributes->GetHP());

		}


		//TODO: other recovery aspects? MP? TP?

		RegenTickTimer = 0.0f;

	}

	RegenTickTimer += 1;

	
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
	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());

		//TODO: make a new attribute function to return the health in a different format?
		HealthBarWidget->SetHealthText(Attributes->GetHealthPercent());
	}
}

//Return the user friendly name of this character
FString ABaseCharacter::GetName()
{
	return Name;
}




