// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Enemy/EnemyTypes.h"
#include "Enemy.generated.h"


class UPawnSensingComponent;
class AAIController;


UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	

protected:
	virtual void BeginPlay() override;

	void SetHealthBarVisibility(bool ShouldBeOn, FString FunctionCallingThis);

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

	/* **********
	* Combat ****
	* ***********
	*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsCombatTickReady;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FTimerHandle CombatTickTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float CombatTickLength = 50.0f;

	UPROPERTY()
	AAIController* AIController;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TSubclassOf<AActor> EnemySecondWeapon;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	double HideDistance = 600.f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Despawn")
	float DespawnTimer = 5.f;

	//use the enemy specific sockets to attach weapon
	UFUNCTION(BlueprintCallable)
	FString BuildWeaponSocketString();

	//if the enemy has multiple weapons, call this in BuildWeaponSocketString to set a second socket
	UFUNCTION(BlueprintCallable)
	FString BuildWeaponSocketStringTwo();

	virtual void UpdateCombatHUD();

	virtual void SetReadyInCombat() override;

	virtual void Recover(float DeltaTime) override;

private:

	void Patrol();
	void MoveToTarget();
	bool IsInRangeOfTarget(AActor* Target, double Radius);


	void ReadyCombatTick();
	void ResetCombatTick();


	//used for Enemy AI Combat decisions
	void Combat();
	virtual void StartCombat() override;
	
	virtual void EndCombat() override;
	virtual void AbortAttack() override;

	bool ShouldDefend();
	bool ShouldDodge();
	bool ShouldHide();
	bool ShouldFlee();
	bool ShouldSpecialMove();
	bool ShouldMeleeAttack();
	bool ShouldRangedAttack();
	bool ShouldSnipeAttack();
	bool ReadyForCombatMove();

	void Flee();
	virtual void MeleeAttack() override;
	virtual void RangedAttack() override;
	virtual void SnipeAttack() override;
	virtual void SpecialAttack() override;
	virtual void Defend() override;
	virtual void Dodge() override;
	virtual void Hide() override;

	virtual void Death() override;

	UPROPERTY(EditAnywhere, Category = Treasure)
	FVector DropLocationOffset = FVector();

	void SpawnDrops();

	UPROPERTY(EditAnywhere, Category = Soul)
	TSubclassOf<class ASoul> SoulClass;

	/*
	* Components
	*/

	
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;
	
};
