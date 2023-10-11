// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Enemy/EnemyTypes.h"
#include "Enemy.generated.h"


class AAIController;
class UPawnSensingComponent;


UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//the distance from combattarget enemy should reach before trying to hide
	double HideDistance();

protected:
	virtual void BeginPlay() override;
	

	UPROPERTY()
	AAIController* EnemyController;


	/*
	* Navigation
	* 
	* AI Movement is implemented in Blueprints. BP_Enemy Tick Function.
	*  
	*  If we eventually want to move it to C++: refer to video 171 4 min
	*  And include AIController.h and use virtual FPathFollowingRequestResult MoveTo
	* 
	*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	EPatrolType PatrolType = EPatrolType::EPT_FullRandom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	AActor* MovementGoal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	TArray<AActor*> ArrayOfPatrolGoals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	AActor* SpawnPoint;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	bool IsChangingDirection = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	double PatrolRadius = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	float PatrolPauseLength = 5.0f; 

	
	void Patrol();

	void MoveToTarget();

	int32 TargetSelection = 0;
	bool IsReversePatrol = false;


	bool IsInRangeOfTarget(AActor* Target, double Radius);



	/* **********
	* Combat ****
	* ***********
	*/

	
	void ReadyCombatTick();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsCombatTickReady;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FTimerHandle CombatTickTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float CombatTickLength = 2.0f;

	bool ShouldDefend();
	bool ShouldDodge();
	bool ShouldHide();
	bool ShouldFlee();
	bool ShouldSpecialMove();
	bool ReadyForCombatMove();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	double CombatRadius = 2500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsReturning = false;


	//this is used to initialize the enemy type, so we cannot use the weapon type from the weapon like we do on player character, therefore this is needed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	EEnemyWeapon EnemyWeaponType = EEnemyWeapon::EEW_Unarmed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TSubclassOf<AActor> EnemyWeapon;

	UFUNCTION(BlueprintCallable)
	AWeapon* GetEnemyWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	double MeleeAttackRadius = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	double RangedAttackRadius = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	double SnipeAttackRadius = 2000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	double SpecialAttackRadius = 200.f;
	
	//Used for Enemy AI decisions
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	ECombatMode CombatMode = ECombatMode::ECM_OutOfCombat;

	void Flee();

	
	//used for Enemy AI Combat decisions
	void Combat();

	virtual void StartCombat() override;
	virtual void EndCombat() override;

	

	//Montages
	//TODO remove. this is in basecharacter
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages")
	UAnimMontage* AttackMontage;*/
	
	

	//use the enemy specific sockets to attach weapon
	UFUNCTION(BlueprintCallable)
	FString BuildWeaponSocketString();

private:

	/*
	* Components
	*/

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	
};
