// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"

#include "AIController.h"

#include "Components/SkeletalMeshComponent.h"

#include "HUD/HealthBarComponent.h"
#include "Components/AttributeComponent.h"
#include "Perception/PawnSensingComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Items/Weapons/Weapon.h"
#include "Items/Soul.h"



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
		SetHealthBarVisibility(false, "BeginPlay");
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
	if (CanSee())
	{
		ASlashCharacter* CharacterSeen = Cast<ASlashCharacter>(SeenPawn);
		if (CharacterSeen && !IsInCombat)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Pawn Seen"));

			if (CharacterSeen->ActorHasTag("Dead"))
			{
				//UE_LOG(LogTemp, Warning, TEXT("Seen Character is dead. Return."));

				//TODO: add to ignore list? would need to remove if revived?

				return;
			};


			SetCombatTarget(CharacterSeen, "PawnSeen");

			CharacterSeen->SetInCombat(true);

			StartCombat();
		}
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
			//if target is alive attack, make a combat move, otherwise, end combat
			if (CombatTarget->GetCombatMode() != ECombatMode::ECM_Dead)
			{
				Combat();
			}
			else
			{
				EndCombat();
			}
		}

		//BP: Logic for PURUSING COMBAT TARGET is in Blueprints
		
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

	if (CombatMode != ECombatMode::ECM_Dead)
	{
		SetHealthBarVisibility(true, "Start Combat");

		SetCombatMode(ECombatMode::ECM_Chasing);
		IsCombatTickReady = true;
	}
	
}

void AEnemy::SetHealthBarVisibility(bool ShouldBeOn, FString FunctionCallingThis)
{
	FString BString = "On";
	if (!ShouldBeOn) { BString = "Off"; }
	UE_LOG(LogTemp, Warning, TEXT("%s: Set Health Bar Visibility : %s"), *FunctionCallingThis, *BString);

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(ShouldBeOn);
	}
}

void AEnemy::EndCombat()
{

	Super::EndCombat();

	if (HealthBarWidget)
	{
		SetHealthBarVisibility(false, "EndCombat");
	}
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

void AEnemy::SetReadyInCombat()
{
	Super::SetReadyInCombat();

	if (IsInCombat)
	{
		SetCombatMode(ECombatMode::ECM_Chasing);
	}
	else
	{
		SetCombatMode(ECombatMode::ECM_OutOfCombat);
	}
}

void AEnemy::Recover(float DeltaTime)
{
	Super::Recover(DeltaTime);

	if (CanRecover())
	{
		if (IsRegening)
		{
			if (HealthBarWidget)
			{
				SetHealthBarVisibility(true, "Recover, IsRegening");
			}
		}
	}
		
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
	
	SpawnDrops();

	//Destroy after delay
	SetLifeSpan(DespawnTimer);

	UnequipWeapon();

}

void AEnemy::SpawnDrops()
{
	//Spawn Exp Drop
	UWorld* World = GetWorld();
	if (World && SoulClass && Attributes)
	{
		FVector SoulSpawnPoint = GetActorLocation() + DropLocationOffset;
		ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass, SoulSpawnPoint, GetActorRotation());
		SpawnedSoul->SetActorEnableCollision(false);
		
		//@yelsa this is causing an error when calling setsouldropamount. debug

		//if (Attributes->GetSoulDropAmount())
		//{
		//	SpawnedSoul->SetValue(Attributes->GetSoulDropAmount());
		//	SpawnedSoul->SetActorEnableCollision(true);
		//}
		//else
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("soul drop amount attribute not set"));
		//}
		
	
	}

	//TODO: spawn treasure too?
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

	SetCombatMode(ECombatMode::ECM_Fleeing);
	//TODO: execute the flee logic

	//TODO: make a check to see if chasing should be turned back on after x time or x percent hp?


	ResetCombatTick();
}

void AEnemy::MeleeAttack()
{
	Super::MeleeAttack();

	ResetCombatTick();
}

void AEnemy::RangedAttack()
{
	Super::RangedAttack();

	ResetCombatTick();
}

void AEnemy::SnipeAttack()
{
	Super::SnipeAttack();

	ResetCombatTick();
}

void AEnemy::SpecialAttack()
{
	Super::SpecialAttack();

	ResetCombatTick();
}

void AEnemy::Defend()
{
	Super::Defend();

	ResetCombatTick();
}

void AEnemy::Dodge()
{
	Super::Dodge();

	ResetCombatTick();
}

void AEnemy::Hide()
{
	Super::Hide();

	ResetCombatTick();
}


void AEnemy::AbortAttack()
{
	Super::AbortAttack();

	SetCombatMode(ECombatMode::ECM_Chasing);
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







