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

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, ASlashCharacter* DamageDealer, float Damage, int32 PrecisionRange, int32 LowAccFloor, int32 HighAccFloor)
{

	CharacterWhoDamagedEnemy = DamageDealer;
	DamagerPrecisionRange = PrecisionRange;
	DamagerLowAccFloor = LowAccFloor;
	DamagerHighAccFloor = HighAccFloor;

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

		FinalDamageAmount = CalculateFinalDamageAmount(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

		Attributes->ReceiveDamage(FinalDamageAmount);
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
		//TODO: make a new attribute function to return the health in a different format?
		HealthBarWidget->SetHealthText(Attributes->GetHealthPercent());
	}

	return FinalDamageAmount;
}


// This function is modifying the base weapon damage using attacker and defender stats. (DamageAmount is base weapon damage.)
float AEnemy::CalculateFinalDamageAmount(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = 0.0f;

	float AttackerSTR;
	float AttackerDEX;
	float DefenderVIT;
	float DefenderAGI;

	if (CharacterWhoDamagedEnemy != nullptr)
	{
		UAttributeComponent* SlashCharacterAttributeComponent = CharacterWhoDamagedEnemy->GetAttributes();
		if (SlashCharacterAttributeComponent)
		{
			AttackerSTR = SlashCharacterAttributeComponent->GetStr();
			AttackerDEX = SlashCharacterAttributeComponent->GetDex();
		}
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("no slash character damager causer"));
	}

	if (Attributes)
	{
		DefenderVIT = Attributes->GetVit();
		DefenderAGI = Attributes->GetAgi();
	}

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
			StrVitCalc = (2/3);
		}
		else
		{
			StrVitCalc = (5/6);
		}
	}

	if (StrVitCalc <= 0) { StrVitCalc = 0.5; }
	if (StrVitCalc >= 2) { StrVitCalc = 2; }

	float PDif = DamageAmount * StrVitCalc;


	UE_LOG(LogTemp, Warning, TEXT("attackstr: %f, defendervit: %f"), AttackerSTR, DefenderVIT);
	UE_LOG(LogTemp, Warning, TEXT("attackdex: %f, defenderagi: %f"), AttackerDEX, DefenderAGI);
	UE_LOG(LogTemp, Warning, TEXT("strvitcalc: %f"), StrVitCalc);
	UE_LOG(LogTemp, Warning, TEXT("pdif: %f"), PDif);

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

	int32 RandomSelection = Floor + FMath::RandRange(0, DamagerPrecisionRange);
	const float RandomSelectionFloat = (float)RandomSelection;
	UE_LOG(LogTemp, Warning, TEXT("randomselection : %f"), RandomSelectionFloat);

	FinalDamageAmount = PDif * (RandomSelectionFloat / 100);
	UE_LOG(LogTemp, Warning, TEXT("enemy hp mod calc.  final dmg amount: %f"), FinalDamageAmount);

	//the final value is then used in TakeDamage to update the HP and set the HPbarwidget
	return FinalDamageAmount;
}

