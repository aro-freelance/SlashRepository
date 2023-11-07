// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"

#include "AIController.h"

#include "Components/SkeletalMeshComponent.h"

#include "HUD/HealthBarComponent.h"
#include "Components/AttributeComponent.h"
#include "Perception/PawnSensingComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Items/Weapons/Weapon.h"



AEnemy::AEnemy()
{
 	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	

	PawnSensing = CreateAbstractDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 2500.f;
	PawnSensing->SetPeripheralVisionAngle(50.f);

	//TODO:create more complex tag system so enemies are not all the same team?
	Tags.Add(FName("Enemy"));

}


void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
		HealthBarWidget->SetNameText(Name);
	}

	if (PawnSensing)
	{
		UE_LOG(LogTemp, Warning, TEXT("base begin pawnsensing setup"));
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}


	GetWorldTimerManager().SetTimer(CombatTickTimer, this, &AEnemy::ReadyCombatTick, CombatTickLength);

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


void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//check if character is in aggro range
	if (CombatTarget && Attributes)
	{
		
		SetFollowDistance();

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
		if (HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(true);
		}

		Recover(DeltaTime);

	}

	//BP: Logic for PATROLLING when out of combat is in Blueprints (refer to 171 12min to implement in C++)
	//Patrol();


}


void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


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


void AEnemy::ResetCombatTick()
{
	IsCombatTickReady = false;
	GetWorldTimerManager().SetTimer(CombatTickTimer, this, &AEnemy::ReadyCombatTick, CombatTickLength);
}

void AEnemy::ReadyCombatTick()
{
	IsCombatTickReady = true;
}




void AEnemy::StartCombat()
{
	Super::StartCombat();

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}

	CombatMode = ECombatMode::ECM_Chasing;
	IsCombatTickReady = true;

}

void AEnemy::EndCombat()
{

	Super::EndCombat();

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}

	CombatMode = ECombatMode::ECM_OutOfCombat;
}



void AEnemy::Combat()
{

	if (ShouldFlee()){ Flee(); }

	else if (ShouldHide()){ Hide(); }

	else if (ShouldDefend()){ Defend(); }

	else if (ShouldDodge()) { Dodge(); }

	else if (ShouldSpecialMove()){ SpecialAttack(); }

	else if (ShouldMeleeAttack()){ MeleeAttack(); }

	else if (ShouldRangedAttack()){ RangedAttack(); }

	else if (ShouldSnipeAttack()){ SnipeAttack(); }

	else { ResetCombatTick(); }

}



void AEnemy::Death()
{
	Super::Death();

	CombatMode = ECombatMode::ECM_Dead;

	

	//Destroy after delay
	SetLifeSpan(DespawnTimer);

	UnequipWeapon();

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
		&& CombatMode != ECombatMode::ECM_Dead
		&& CombatMode != ECombatMode::ECM_Hiding 
		&& CombatMode != ECombatMode::ECM_OutOfCombat)
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
	if (IsInRangeOfTarget(CombatTarget, SpecialAttackRadius) 
		&& Attributes->GetTP() >= SpecialAttackTPCost 
		&& CombatMode == ECombatMode::ECM_Chasing)
	{
		b = true;
	}

	return b;
}

bool AEnemy::ShouldMeleeAttack()
{
	bool b = false;

	if (IsInRangeOfTarget(CombatTarget, MeleeAttackRadius) && HasMeleeWeapon && CombatMode == ECombatMode::ECM_Chasing)
	{
		b = true;
	}

	return b;
}

bool AEnemy::ShouldRangedAttack()
{
	bool b = false;

	if (IsInRangeOfTarget(CombatTarget, RangedAttackRadius) && HasRangedWeapon && CombatMode == ECombatMode::ECM_Chasing)
	{
		b = true;
	}

	return b;
}

bool AEnemy::ShouldSnipeAttack()
{
	bool b = false;

	if (IsInRangeOfTarget(CombatTarget, SnipeAttackRadius) && HasSnipeWeapon && CombatMode == ECombatMode::ECM_Chasing)
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


	ResetCombatTick();
}

void AEnemy::MeleeAttack()
{
	Super::MeleeAttack();

	CombatMode = ECombatMode::ECM_MeleeAttacking;

	ResetCombatTick();
}

void AEnemy::RangedAttack()
{
	Super::RangedAttack();

	CombatMode = ECombatMode::ECM_RangeAttacking;

	ResetCombatTick();
}

void AEnemy::SnipeAttack()
{
	Super::SnipeAttack();

	CombatMode = ECombatMode::ECM_SnipeAttacking;

	ResetCombatTick();
}

void AEnemy::SpecialAttack()
{
	Super::SpecialAttack();

	CombatMode = ECombatMode::ECM_SpecialAttacking;

	ResetCombatTick();
}

void AEnemy::Defend()
{
	Super::Defend();

	CombatMode = ECombatMode::ECM_Defending;

	ResetCombatTick();
}

void AEnemy::Dodge()
{
	Super::Dodge();

	CombatMode = ECombatMode::ECM_Dodging;

	ResetCombatTick();
}

void AEnemy::Hide()
{
	Super::Hide();

	CombatMode = ECombatMode::ECM_Hiding;

	ResetCombatTick();
}


void AEnemy::AbortAttack()
{
	Super::AbortAttack();

	CombatMode = ECombatMode::ECM_Chasing;
}


//this is used to initialize the enemy type, so we cannot use the weapon type from the weapon like we do on player character, therefore this is needed
FString AEnemy::BuildWeaponSocketString()
{
	//TODO: update these when there are new sockets added

	FString SocketString = FString();

	switch (EnemyWeaponType)
	{
	case EEnemyWeapon::EEW_Unarmed:
		SocketString = "RightHandSocket";
		break;
	case EEnemyWeapon::EEW_Sword:
		SocketString = "RightHandSocket";
		break;
	case EEnemyWeapon::EEW_GreatHammer:
		SocketString = "RightHandSocket";
		break;
	case EEnemyWeapon::EEW_Rifle:
		SocketString = "RightHandSocket";
		break;
	case EEnemyWeapon::EEW_Pistol:
		SocketString = "RightHandSocket";
		break;
	case EEnemyWeapon::EEW_Bow:
		SocketString = "RightHandSocket";
		break;
	default:
		break;
	}


	return SocketString;
}




AWeapon* AEnemy::GetEnemyWeapon()
{

	AWeapon* WeaponRef = Cast<AWeapon>(EnemyWeapon);

	return WeaponRef;

}







