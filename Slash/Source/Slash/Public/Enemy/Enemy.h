// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy/EnemyTypes.h"
#include "Enemy.generated.h"


class UAnimMontage;
class UAttributeComponent;
class UHealthBarComponent;
class AAIController;
class UPawnSensingComponent;


UCLASS()
class SLASH_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, ASlashCharacter* DamageDealer, AWeapon* Weapon);
	

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void Death();

	FString GetName();

	double HideDistance();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effects")
	FRotator HitParticleRotation = FRotator(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effects")
	FVector HitParticleScale = FVector(1.f, 1.f, 1.f);

	/*
	* Play montage functions
	*/

	void PlayHitReactMontage(const FName& SectionName);
	void PlayDeathMontage(const FName& SectionName);

	float CalculatePhysicalDamage(float DamageAmount);
	float CalculateMagicalDamage(float DamageAmount);


	void UpdateCombatHUD();

	UPROPERTY()
	AAIController* EnemyController;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Name")
	FString EnemyName = "Default Enemy Name";

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
	bool HasRangedWeapon = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool HasSnipeWeapon = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsInCombat = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsRegening = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsReturning = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	double MeleeAttackRadius = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	double RangedAttackRadius = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	double SnipeAttackRadius = 2000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	double SpecialAttackRadius = 200.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	ECombatMode CombatMode = ECombatMode::ECM_OutOfCombat;

	void MeleeAttack();
	void RangedAttack();
	void SnipeAttack();
	void SpecialAttack();
	void Defend();
	void Dodge();
	void Flee();
	void Hide();
	
	void Combat();

	void StartCombat();
	void EndCombat();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float MaxHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float RegenPercent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float TPGainPercent = 0.05;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float SpecialAttackTPCost = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float FleeHPPercent = 0.15f;
	
	void IncreaseTP();

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

	//the most recent character to damage this Enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damaged By Properties")
	ASlashCharacter* CombatTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damaged By Properties")
	AWeapon* WeaponThatDamagedEnemy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damaged By Properties")
	FVector LastHitImpactPoint = FVector();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damaged By Properties")
	FName LastHitDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damaged By Properties")
	float LastDamageAmount;
	
	bool CheckCritical(const FVector& ImpactPoint);
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regen")
	float RegenTickLength = 150.0f;

	UFUNCTION(BlueprintCallable)
	void Recover();

private:

	/*
	* Components
	*/

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	float CompareSTRVIT(float AttackerSTR, float DefenderVIT);
	int32 CompareDEXAGI(float AttackerDEX, float DefenderAGI);
	float CompareINTMND(float AttackerINT, float DefenderMND);
	int32 CompareCHR(float AttackerCHR, float DefenderCHR);


	float RegenTickTimer = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Visual Effects")
	UParticleSystem* HitParticles;

	

	/*
	* Animation Montages
	*/

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	FName CalculateHitReactSectionName(const FVector& ImpactPoint);
	FName CalculateDeathMontageSectionName();
	
	
};
