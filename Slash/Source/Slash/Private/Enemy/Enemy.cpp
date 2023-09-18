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



void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, ASlashCharacter* DamageDealer, float Damage, int32 PrecisionRange, int32 LowAccFloor, int32 HighAccFloor, float PercentMagicDamage, const FString& WeaponName)
{

	//Store Information about the Attacker and Weapon of Attacker
	CharacterWhoDamagedEnemy = DamageDealer;
	DamagerPrecisionRange = PrecisionRange;
	DamagerLowAccFloor = LowAccFloor;
	DamagerHighAccFloor = HighAccFloor;
	DamagerPercentMagicDamage = PercentMagicDamage;
	DamagerWeaponName = WeaponName;

	DirectionalHitReact(ImpactPoint);

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

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
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


	UE_LOG(LogTemp, Warning, TEXT("Angle of Impact: %d"), AngleOfImpact);

	PlayHitReactMontage(SectionName);

	////debug arrows for forward and impactpoint vectors
	////context, start, end vector (multiplied to increase length),  arrow head size, color, duration
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 5.f,
	//	FColor::Red, 5.f);
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ImpactPointVector * 60.f, 5.f,
	//	FColor::Blue, 5.f);

}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = 0.0f;

	if (Attributes && HealthBarWidget)
	{
		//Calculate Damage to Deal to HP
		if (DamagerPercentMagicDamage == 1) 
		{
			FinalDamageAmount = CalculateMagicalDamage(DamageAmount);
			UE_LOG(LogTemp, Warning, TEXT("magic damage. %f"), FinalDamageAmount);
		}
		else if (DamagerPercentMagicDamage == 0)
		{
			FinalDamageAmount = CalculatePhysicalDamage(DamageAmount);
			UE_LOG(LogTemp, Warning, TEXT("physical damage. %f"), FinalDamageAmount);
		}
		else
		{	
			float MagicDamageAmount = DamageAmount * DamagerPercentMagicDamage;
			float PhysicalDamageAmount = DamageAmount * (1 - DamagerPercentMagicDamage);

			FinalDamageAmount = CalculateMagicalDamage(MagicDamageAmount) + CalculatePhysicalDamage(PhysicalDamageAmount);
			UE_LOG(LogTemp, Warning, TEXT("hybrid damage. Final: %f. Magic: %f. Physical: %f."), FinalDamageAmount, MagicDamageAmount, PhysicalDamageAmount);
		}

		//Update HP Amount
		Attributes->ReceiveDamage(FinalDamageAmount);

		//Update Health Bar and other HUD elements
		DamageTakenUpdateHUD();


		if (GEngine)
		{
			int32 FinalDamageAmountRounded = FMath::RoundToInt(FinalDamageAmount);
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("%s was hit by %s using %s for %i damage"), *EnemyName, *CharacterWhoDamagedEnemy->GetName(), *DamagerWeaponName, FinalDamageAmountRounded));
		}

		UE_LOG(LogTemp, Warning, TEXT("%s was hit by %s using %s for %f damage"), *EnemyName, *CharacterWhoDamagedEnemy->GetName(), *DamagerWeaponName, FinalDamageAmount);
	}



	return FinalDamageAmount;
}


// This function is modifying the base weapon damage using attacker and defender stats. (DamageAmount is base weapon damage.)
float AEnemy::CalculatePhysicalDamage(float DamageAmount)
{
	float FinalDamageAmount = 0.0f;

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
			int32 RandomSelection = Floor + FMath::RandRange(0, DamagerPrecisionRange);

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
		Floor = DamagerHighAccFloor;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("agi > dex"));
		Floor = DamagerLowAccFloor;
	}

	return Floor;
}

float AEnemy::CalculateMagicalDamage(float DamageAmount)
{
	float FinalDamageAmount = 0.0f;

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
			int32 RandomSelection = Floor + FMath::RandRange(0, DamagerPrecisionRange);

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
		Floor = DamagerHighAccFloor;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("defender wins chr"));
		Floor = DamagerLowAccFloor;
	}

	return Floor;
}

void AEnemy::DamageTakenUpdateHUD()
{
	HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());

	//TODO: make a new attribute function to return the health in a different format?
	HealthBarWidget->SetHealthText(Attributes->GetHealthPercent());

}


//Return the user friendly name of this enemy
FString AEnemy::GetName()
{
	return EnemyName;
}
