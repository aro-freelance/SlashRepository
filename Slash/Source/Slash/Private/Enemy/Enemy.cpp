// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"
#include "Slash/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"


AEnemy::AEnemy()
{
 	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	

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

void AEnemy::GetHit(const FVector& ImpactPoint)
{

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

