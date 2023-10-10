// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Characters/SlashCharacter.h"
#include "AIController.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Perception/PawnSensingComponent.h"

#include "Animation/AnimMontage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Items/Weapons/Weapon.h"

#include "Slash/DebugMacros.h"



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

	PawnSensing = CreateAbstractDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 2500.f;
	PawnSensing->SetPeripheralVisionAngle(50.f);


}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
		HealthBarWidget->SetNameText(EnemyName);
	}

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);

	}


	GetWorldTimerManager().SetTimer(CombatTickTimer, this, &AEnemy::ReadyCombatTick, CombatTickLength);

	//BP: Movement is Currently Handled in Blueprint (refer to 171 4min to implement in C++)
	/*if (ArrayOfPatrolGoals.Num() > 0)
	{
		MovementGoal = ArrayOfPatrolGoals[0];
		MoveToTarget();
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::MoveToTarget, PatrolPauseLength);
	}*/
	

}


void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//check if character is in aggro range
	if (CombatTarget && Attributes)
	{

		//TODO: Fully Implement EnemyTypes ECombatMode or remove it. To implement fully I will need to integrate it in blueprints.

		//if out of combat distance, end combat
		if (!IsInRangeOfTarget(CombatTarget, CombatRadius))
		{
			EndCombat();
		}
		//Wait to make a new Combat choice every tick
		else if (IsCombatTickReady) 
		{ 
			Combat();
		}

		//BP: Logic for PURUSING COMBAT TARGET is in Blueprints
		
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

	//BP: Logic for PATROLLING when out of combat is in Blueprints (refer to 171 12min to implement in C++)
	//Patrol();


}


void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

/*
* MOVEMENT FUNCTIONS
*/

//BP: This functionality is in blueprints. This function is not currently in use. It is here in case I decide to switch to C++. 
/*void AEnemy::Patrol()
{
	UE_LOG(LogTemp, Warning, TEXT("c++ patrol "));

	//1. SET TARGET//////////////////////////////////////////////////////

	const int32 NumPatrolTargets = ArrayOfPatrolGoals.Num();

	if (PatrolType == EPatrolType::EPT_FullRandom)
	{
		//TODO: move to random location inside of nav
	}

	//RANDOM IN SET
	if (MovementGoal && PatrolType == EPatrolType::EPT_RandomInSet)
	{
		if (IsInRangeOfTarget(MovementGoal, PatrolRadius))
		{
			if (NumPatrolTargets > 0)
			{
				TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
				MovementGoal = ArrayOfPatrolGoals[TargetSelection];

			}
		}
	}

	if (MovementGoal && PatrolType == EPatrolType::EPT_SetOrderLoopEnd)
	{
		if (IsInRangeOfTarget(MovementGoal, PatrolRadius))
		{

			if (NumPatrolTargets > 0)
			{
				MovementGoal = ArrayOfPatrolGoals[TargetSelection];

				//Go to next target
				TargetSelection = TargetSelection + 1;
				//if you reach the end, go to the first target
				if (!ArrayOfPatrolGoals.IsValidIndex(TargetSelection)) { TargetSelection = 0; }
				
			}
		}
	}

	if (MovementGoal && PatrolType == EPatrolType::EPT_SetOrderReverseEnd)
	{

		UE_LOG(LogTemp, Warning, TEXT("order reverse end patrol"));

		if (IsInRangeOfTarget(MovementGoal, PatrolRadius))
		{
			UE_LOG(LogTemp, Warning, TEXT("goal in range"));

			if (NumPatrolTargets > 0)
			{
				MovementGoal = ArrayOfPatrolGoals[TargetSelection];

				UE_LOG(LogTemp, Warning, TEXT("new goal set"));

				//Go to next target
				if (IsReversePatrol) 
				{
					TargetSelection = TargetSelection - 1;
				}
				else 
				{
					TargetSelection = TargetSelection + 1;
				}

				//if you reach the end, reverse
				if (!ArrayOfPatrolGoals.IsValidIndex(TargetSelection)) 
				{ 
					if (IsReversePatrol)
					{
						TargetSelection = TargetSelection + 2;
						IsReversePatrol = false;
					}
					else
					{
						TargetSelection = TargetSelection - 2;
						IsReversePatrol = true;
					}
				}

			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////

	//2. Pause and then Move to Target /////////////

	MoveToTarget();
	//GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::MoveToTarget, PatrolPauseLength);

	///////////////////////////
}
*/

/*
void AEnemy::MoveToTarget()
{
	UE_LOG(LogTemp, Warning, TEXT("move to target called"));

	EnemyController = Cast<AAIController>(GetController());

	if (EnemyController == nullptr || MovementGoal == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("enemy controller or movementgoal null"));
		return;
	}

	if (EnemyController && MovementGoal)
	{
		UE_LOG(LogTemp, Warning, TEXT("enemy controller and goal exist. moving to target"));
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(MovementGoal);
		MoveRequest.SetAcceptanceRadius(15.f);
		FNavPathSharedPtr NavPath;
		EnemyController->MoveTo(MoveRequest, &NavPath);
		TArray<FNavPathPoint>& PathPoints = NavPath->GetPathPoints();
		for (auto& Point : PathPoints)
		{
			const FVector& Location = Point.Location;
			DrawDebugSphere(GetWorld(), Location, 12.f, 12, FColor::Green, false, 10.f);
		}
	}
}
*/

bool AEnemy::IsInRangeOfTarget(AActor* Target, double Radius)
{
	if (Target == nullptr) { return false; }

	bool IsInRange = false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	
	//DRAW_SPHERE_SINGLE_FRAME(GetActorLocation(), FColor::Green);
	//DRAW_SPHERE_SINGLE_FRAME(Target->GetActorLocation(), FColor::Red);

	if (DistanceToTarget <= Radius)
	{
		IsInRange = true;
	}

	return IsInRange;
}


/*
* COMBAT FUNCTIONS
*/


void AEnemy::ReadyCombatTick()
{
	IsCombatTickReady = true;
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{

	ASlashCharacter* CharacterSeen = Cast<ASlashCharacter>(SeenPawn);

	if (CharacterSeen && !IsInCombat)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pawn Seen"));

		CombatTarget = CharacterSeen;

		StartCombat();

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
	CombatTarget = nullptr;
	WeaponThatDamagedEnemy = nullptr;
	LastHitImpactPoint = FVector();
	LastHitDirection = FName();
	LastDamageAmount = 0.f;
	IsInCombat = false;
	CombatMode = ECombatMode::ECM_OutOfCombat;

	//TODO: Turn off combat music
}

void AEnemy::StartCombat()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}

	IsInCombat = true;
	CombatMode = ECombatMode::ECM_Chasing;

	//TODO: Turn on combat music
}

void AEnemy::Combat()
{

	UE_LOG(LogTemp, Warning, TEXT("c++ combat method"));

	//if mid-attack/defend/dodge or out of combat, reset the combat tick and end this function
	//if (!ReadyForCombatMove) 
	//{
	//	IsCombatTickReady = false;
	//	GetWorldTimerManager().SetTimer(CombatTickTimer, this, &AEnemy::ReadyCombatTick, CombatTickLength);
	//	return; 
	//}

	//else 
	if (ShouldFlee()){ Flee(); }

	else if (ShouldHide()){ Hide(); }

	else if (ShouldDefend()){ Defend(); }

	else if (ShouldDodge()) { Dodge(); }

	else if (ShouldSpecialMove()){ SpecialAttack(); }

	else if (IsInRangeOfTarget(CombatTarget, MeleeAttackRadius)){ MeleeAttack(); }

	else if (IsInRangeOfTarget(CombatTarget, RangedAttackRadius) && HasRangedWeapon){ RangedAttack(); }

	else if (IsInRangeOfTarget(CombatTarget, SnipeAttackRadius) && HasSnipeWeapon){ SnipeAttack(); }

	IsCombatTickReady = false;
	GetWorldTimerManager().SetTimer(CombatTickTimer, this, &AEnemy::ReadyCombatTick, CombatTickLength);

}

bool AEnemy::ShouldDefend()
{
	//TODO: build this logic
	return false;
}

bool AEnemy::ShouldDodge()
{
	//TODO: build this logic
	return false;
}

bool AEnemy::ShouldHide()
{
	//TODO: build this logic. should include distance threshold and hp to hide threshold?
	return false;
}

bool AEnemy::ShouldFlee()
{
	bool b = false;
	if (Attributes->GetHealthPercent() <= FleeHPPercent
		&& CombatMode != ECombatMode::ECM_Hiding && CombatMode != ECombatMode::ECM_OutOfCombat)
	{
		b = true;
	}

	return b;
}

bool AEnemy::ShouldSpecialMove()
{
	//TODO: Add more logic to when special moves are used?

	bool b = false;

	//if in range and has tp
	if (IsInRangeOfTarget(CombatTarget, SpecialAttackRadius) && Attributes->GetTP() >= SpecialAttackTPCost)
	{
		b = true;
	}

	return b;
}

bool AEnemy::ReadyForCombatMove()
{
	bool b = false;


	//TODO: for this to work, we need to switch the enum in Unreal after the anim ends. back to chasing.

	if (CombatMode != ECombatMode::ECM_MeleeAttacking ||
		CombatMode != ECombatMode::ECM_RangeAttacking ||
		CombatMode != ECombatMode::ECM_SnipeAttacking ||
		CombatMode != ECombatMode::ECM_SpecialAttacking ||
		CombatMode != ECombatMode::ECM_Defending ||
		CombatMode != ECombatMode::ECM_Dodging ||
		CombatMode != ECombatMode::ECM_OutOfCombat) 
	{
		b = true;
	}


	return b;
}


void AEnemy::Flee()
{
	UE_LOG(LogTemp, Warning, TEXT("Flee Method"));

	CombatMode = ECombatMode::ECM_Fleeing;
	//TODO: execute the flee logic

	//TODO: make a check to see if chasing should be turned back on after x time or x percent hp?
	
}

void AEnemy::Defend()
{
	UE_LOG(LogTemp, Warning, TEXT("Defend Method"));

	CombatMode = ECombatMode::ECM_Defending;
	//TODO: execute the defend logic

	//TODO: turn chasing back on when defend anim is done?
	
}

void AEnemy::Dodge()
{
	UE_LOG(LogTemp, Warning, TEXT("Dodge Method"));

	CombatMode = ECombatMode::ECM_Dodging;
	//TODO: execute the dodge logic

	//TODO: turn chasing back on when dodge anim is done?
	
}

void AEnemy::Hide()
{
	UE_LOG(LogTemp, Warning, TEXT("Hide Method"));

	CombatMode = ECombatMode::ECM_Hiding;
	//TODO: execute the hide logic

	//TODO: make a check to see if can attack from here, flee or stay here
}

void AEnemy::SpecialAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("SpecialAttack Method"));

	//use the TP
	Attributes->SetTP(Attributes->GetTP() - SpecialAttackTPCost);

	CombatMode = ECombatMode::ECM_SpecialAttacking;
	//TODO: do the attack
	
	bool AttackHitTarget = true;
	//TODO: update this based on the attack hitting or not
	if (AttackHitTarget)
	{
		IncreaseTP();
	}

	//TODO: turn chasing back on when attack anim is done?
}

void AEnemy::MeleeAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("MeleeAttack Method"));

	CombatMode = ECombatMode::ECM_MeleeAttacking;
	//TODO: do the attack


	bool AttackHitTarget = true;
	//TODO: update this based on the attack hitting or not
	if (AttackHitTarget)
	{
		IncreaseTP();
	}

	//TODO: turn chasing back on when attack anim is done?
}

void AEnemy::RangedAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("RangedAttack Method"));

	CombatMode = ECombatMode::ECM_RangeAttacking;
	//TODO: do the attack
	

	bool AttackHitTarget = true;
	//TODO: update this based on the attack hitting or not
	if (AttackHitTarget)
	{
		IncreaseTP();
	}

	//TODO: turn chasing back on when attack anim is done?
}

void AEnemy::SnipeAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("SnipeAttack Method"));

	CombatMode = ECombatMode::ECM_SnipeAttacking;
	//TODO: do the attack
	

	bool AttackHitTarget = true;
	//TODO: update this based on the attack hitting or not
	if (AttackHitTarget)
	{
		IncreaseTP();
	}

	//TODO: turn chasing back on when attack anim is done?
}



void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, ASlashCharacter* DamageDealer, AWeapon* Weapon)
{

	UE_LOG(LogTemp, Warning, TEXT("Get Hit."));

	//Store Information about the Attacker and Weapon of Attacker
	CombatTarget = DamageDealer;
	WeaponThatDamagedEnemy = Weapon;
	LastHitImpactPoint = ImpactPoint;
	LastHitDirection = CalculateHitReactSectionName(ImpactPoint);

	StartCombat();

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

}

void AEnemy::IncreaseTP()
{
	float TP = Attributes->GetTP();
	float MaxTP = Attributes->GetMaxTP();
	float Amount = FMath::Clamp(TP + (MaxTP * TPGainPercent), 0.0f, MaxTP);
	Attributes->SetTP(Amount);
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
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s defeated %s"), *CombatTarget->GetName(), *EnemyName));
	}

	UE_LOG(LogTemp, Warning, TEXT("%s defeated %s"), *CombatTarget->GetName(), *EnemyName);

	FName SectionName = CalculateDeathMontageSectionName();
	PlayDeathMontage(SectionName);

	//Turn off this Enemy's HUD info, combat behaviors
	EndCombat();

	//turn off the collision capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Destroy after delay
	SetLifeSpan(5.f);
}



/*
* COMBAT MATH FUNCTIONS
*/

// This function is modifying the base weapon damage using attacker and defender stats. (DamageAmount is base weapon damage.)
float AEnemy::CalculatePhysicalDamage(float DamageAmount)
{
	float FinalDamageAmount = 0.0f;

	int32 PrecisionRange = WeaponThatDamagedEnemy->GetPrecisionRange();

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

//TODO: Calculate if the hit is critical
bool AEnemy::CheckCritical(const FVector& ImpactPoint)
{
	return false;
}

FString AEnemy::BuildWeaponSocketString()
{
	//TODO: update these when there are new sockets added

	FString SocketString = FString();

	switch (WeaponType)
	{
	case EWeaponType::EWT_Unarmed:
		SocketString = "RightHandSocket";
		break;
	case EWeaponType::EWT_Sword:
		SocketString = "RightHandSocket";
		break;
	case EWeaponType::EWT_GreatHammer:
		SocketString = "RightHandSocket";
		break;
	case EWeaponType::EWT_Rifle:
		SocketString = "RightHandSocket";
		break;
	case EWeaponType::EWT_Pistol:
		SocketString = "RightHandSocket";
		break;
	case EWeaponType::EWT_Bow:
		SocketString = "RightHandSocket";
		break;
	default:
		break;
	}


	return SocketString;
}


/*
* UI FUNCTIONS
*/

void AEnemy::UpdateCombatHUD()
{

	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());

		//TODO: make a new attribute function to return the health in a different format?
		HealthBarWidget->SetHealthText(Attributes->GetHealthPercent());
	}

}


/*
* MONTAGE FUNCTIONS
*/


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


/*
* UTILITY FUNCTIONS
*/

//Return the user friendly name of this enemy
FString AEnemy::GetName()
{
	return EnemyName;
}

AWeapon* AEnemy::GetEnemyWeapon()
{

	AWeapon* WeaponRef = Cast<AWeapon>(EnemyWeapon);

	return WeaponRef;

}







