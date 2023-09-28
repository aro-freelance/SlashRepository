// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"
#include "Slash/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Characters/SlashCharacter.h"
#include "Items/Weapons/Weapon.h"



AEnemy::AEnemy()
{
 	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());


}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
		HealthBarWidget->SetNameText(EnemyName);
	}
}


void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//check if character is in aggro range
	if (CharacterWhoDamagedEnemy)
	{
		const double DistanceToTarget = (CharacterWhoDamagedEnemy->GetActorLocation() - GetActorLocation()).Size();

		if (DistanceToTarget > CombatRadius)
		{
			EndCombat();
		}

		//TODO: else pursue target
	}


	if (IsRegening)
	{
		float RegenTickTimerDeltaTime = RegenTickTimer * DeltaTime;
		float RegenTickLengthDeltaTime = RegenTickLength * DeltaTime;
		
		if (HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(true);
		}

		if (RegenTickTimerDeltaTime >= RegenTickLengthDeltaTime)
		{
			Recover();

			RegenTickTimer = 0.0f;

		}

		RegenTickTimer += 1;

	}


}

void AEnemy::EndCombat()
{
	//hide combat HUD
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}

	//clear last target information
	CharacterWhoDamagedEnemy = nullptr;
	WeaponThatDamagedEnemy = nullptr;
	LastHitImpactPoint = FVector();
	LastHitDirection = FName();
	LastDamageAmount = 0.f;
	IsInCombat = false;
}

void AEnemy::Recover()
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
			float Amount = LocalMaxHP * CurrentRegenPercent;
			float ClampedAmount = FMath::Clamp(CurrentHP + Amount, 0.0f, LocalMaxHP);

			//change the stat amount
			Attributes->SetHP(ClampedAmount);

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

}


void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HitReactMontage)
	{

		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);

	}
}

void AEnemy::PlayDeathMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && DeathMontage)
	{

		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);

	}
}


void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, ASlashCharacter* DamageDealer, AWeapon* Weapon)
{

	//Store Information about the Attacker and Weapon of Attacker
	CharacterWhoDamagedEnemy = DamageDealer;
	WeaponThatDamagedEnemy = Weapon;
	LastHitImpactPoint = ImpactPoint;
	LastHitDirection = CalculateHitReactSectionName(ImpactPoint);
	
	IsInCombat = true;

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

		PlayHitReactMontage(LastHitDirection);

		if (HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(true);
		}

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



FName AEnemy::CalculateHitReactSectionName(const FVector& ImpactPoint)
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


float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = 0.0f;

	if (Attributes && HealthBarWidget && WeaponThatDamagedEnemy)
	{
		//get the weapon's magic damage to physical damage ratio
		float PercentMagicDamage = WeaponThatDamagedEnemy->GetPercentMagicDamage();
		
		//handle critical hits
		bool isCritical = CheckCritical(LastHitImpactPoint);
		if (isCritical) { DamageAmount = (DamageAmount * WeaponThatDamagedEnemy->GetCriticalMultiplier()); }

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
		
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("%s was hit by %s for %i damage"), *EnemyName, *WeaponThatDamagedEnemy->WeaponName, FinalDamageAmountRounded));

		}

		//UE_LOG(LogTemp, Warning, TEXT("%s was hit by %s using %s for %f damage"), *EnemyName, *CharacterWhoDamagedEnemy->GetName(), *DamagerWeaponName, LastDamageAmount);


		//Update HP Amount
		Attributes->ReceiveDamage(FinalDamageAmount);

		//Update Health Bar and other HUD elements
		UpdateCombatHUD();

	}

	return FinalDamageAmount;
}

void AEnemy::Death()
{
	//Log Message for Defeat
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s defeated %s"), *CharacterWhoDamagedEnemy->GetName(), *EnemyName));
	}

	UE_LOG(LogTemp, Warning, TEXT("%s defeated %s"), *CharacterWhoDamagedEnemy->GetName(), *EnemyName);

	FName SectionName = CalculateDeathMontageSectionName();
	PlayDeathMontage(SectionName);

	//Turn off this Enemy's HUD info, combat behaviors
	EndCombat();

	//turn off the collision capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Destroy after delay
	SetLifeSpan(5.f);
}




// This function is modifying the base weapon damage using attacker and defender stats. (DamageAmount is base weapon damage.)
float AEnemy::CalculatePhysicalDamage(float DamageAmount)
{
	float FinalDamageAmount = 0.0f;

	int32 PrecisionRange = WeaponThatDamagedEnemy->GetPrecisionRange();

	if (CharacterWhoDamagedEnemy != nullptr && Attributes)
	{
		UAttributeComponent* SlashCharacterAttributeComponent = CharacterWhoDamagedEnemy->GetAttributes();
		if (SlashCharacterAttributeComponent)
		{
			float AttackerSTR = SlashCharacterAttributeComponent->GetStr();
			float AttackerDEX = SlashCharacterAttributeComponent->GetDex();
			float DefenderVIT = Attributes->GetVit();
			float DefenderAGI = Attributes->GetAgi();

			float PDif = DamageAmount * CompareSTRVIT(AttackerSTR, DefenderVIT);
			int32 Floor = CompareDEXAGI(AttackerDEX, DefenderAGI);
			int32 RandomSelection = Floor + FMath::RandRange(0, PrecisionRange);

			FinalDamageAmount = PDif * ((float)RandomSelection/ 100);
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


float AEnemy::CompareSTRVIT(float AttackerSTR, float DefenderVIT)
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

int32 AEnemy::CompareDEXAGI(float AttackerDEX, float DefenderAGI)
{
	int32 Floor;

	//if the attacker is more accurate than the enemy's evasion they use a more favorable random damage calculation
	if (AttackerDEX > DefenderAGI)
	{
		UE_LOG(LogTemp, Warning, TEXT("dex > agi"));
		Floor = WeaponThatDamagedEnemy->GetHighAccFloor();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("agi > dex"));
		Floor = WeaponThatDamagedEnemy->GetLowAccFloor();
	}

	return Floor;
}

float AEnemy::CalculateMagicalDamage(float DamageAmount)
{
	float FinalDamageAmount = 0.0f;

	int32 PrecisionRange = WeaponThatDamagedEnemy->GetPrecisionRange();

	if (CharacterWhoDamagedEnemy != nullptr && Attributes)
	{
		UAttributeComponent* SlashCharacterAttributeComponent = CharacterWhoDamagedEnemy->GetAttributes();
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

float AEnemy::CompareINTMND(float AttackerINT, float DefenderMND)
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

int32 AEnemy::CompareCHR(float AttackerCHR, float DefenderCHR)
{
	int32 Floor;

	//if the attacker is more accurate than the enemy's evasion they use a more favorable random damage calculation
	if (AttackerCHR > DefenderCHR)
	{
		UE_LOG(LogTemp, Warning, TEXT("attacker wins chr"));
		Floor = WeaponThatDamagedEnemy->GetHighAccFloor();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("defender wins chr"));
		Floor = WeaponThatDamagedEnemy->GetLowAccFloor();
	}

	return Floor;
}

void AEnemy::UpdateCombatHUD()
{
	
	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());

		//TODO: make a new attribute function to return the health in a different format?
		HealthBarWidget->SetHealthText(Attributes->GetHealthPercent());
	}
	
}


//TODO: Calculate if the hit is critical
bool AEnemy::CheckCritical(const FVector& ImpactPoint)
{
	return false;
}


FName AEnemy::CalculateDeathMontageSectionName()
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


//Return the user friendly name of this enemy
FString AEnemy::GetName()
{
	return EnemyName;
}
