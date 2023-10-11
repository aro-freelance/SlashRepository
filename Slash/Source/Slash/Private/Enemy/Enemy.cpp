// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "AIController.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/WidgetComponent.h"


#include "Items/Weapons/Weapon.h"

#include "Slash/DebugMacros.h"

//#include "Animation/AnimMontage.h"
//#include "Kismet/KismetSystemLibrary.h"
//#include "Kismet/GameplayStatics.h"




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
		HealthBarWidget->SetNameText(Name);
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

void AEnemy::StartCombat()
{
	Super::StartCombat();

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}

	CombatMode = ECombatMode::ECM_Chasing;

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

	UE_LOG(LogTemp, Warning, TEXT("enemy C++ combat method"));

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

//this is a check on whether the Enemy AI should allow the enemy to attack
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







