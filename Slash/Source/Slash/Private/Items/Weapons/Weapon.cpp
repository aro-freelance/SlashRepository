// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"


//constructor
AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());

	WeaponBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}



void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::Fire()
{
	//get the character that the weapon is attached to
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(GetAttachParentActor());

	if (SlashCharacter == nullptr || SlashCharacter->GetController() == nullptr)
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		APlayerController* PlayerController = Cast<APlayerController>(SlashCharacter->GetController());
		const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset) + 100.f;

		const FVector CharacterLocation = SlashCharacter->GetActorLocation();

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		const FActorSpawnParameters cActorSpawnParams = ActorSpawnParams;


		AActor* A = World->SpawnActor(Projectile);
		A->SetActorLocation(CharacterLocation + (SlashCharacter->GetActorForwardVector()*0.05));
		A->SetOwner(this);


		

	}
		

	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, SlashCharacter->GetActorLocation());
	}

	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		//TODO: play fire anim
	}

}

bool AWeapon::CanFire()
{
	bool b = false;
	
	if (WeaponSize == EWeaponSize::EWS_Rifle  ||
		WeaponSize == EWeaponSize::EWS_Pistol ||
		WeaponSize == EWeaponSize::EWS_Bow    ||
		OverrideCanFire)
	{
		b = true;
	}
	
	return b;
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);

	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (NiagaraEmberEffect)
	{
		NiagaraEmberEffect->Deactivate();
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, FString::Printf(TEXT("%s equipped."), *WeaponName));
	}
		

}

void AWeapon::DetachMeshFromSocket()
{
	FDetachmentTransformRules DeTransformRules(EDetachmentRule::KeepWorld, true);
	ItemMesh->DetachFromComponent(DeTransformRules);

	ItemMesh->SetWorldRotation(StartingRotation);

	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	if (NiagaraEmberEffect)
	{
		NiagaraEmberEffect->Activate();
	}

}



void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

}

//perform a box trace to return information about collisions
void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if(CanDoDamage())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Can do damage. Overlapped with Actor: %s . Component: %s"), *OtherActor->GetName(), *OtherComp->GetName());

		FHitResult BoxHit;
		BoxTrace(BoxHit);
		FString BoxHitString = BoxHit.ToString();

		//if an actor was hit
		if (BoxHit.GetActor())
		{
			//UE_LOG(LogTemp, Warning, TEXT("Hit! Actor: %s. Component: %s."), *BoxHit.GetActor()->GetName(), *BoxHit.GetComponent()->GetName());
	
			IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());

			//get the character that the weapon is attached to
			ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(GetAttachParentActor());

			if (HitInterface && SlashCharacter)
			{
				//call GetHit on the actor that was hit
				HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, SlashCharacter, this);
			}

			//ignore the hit actor so it cannot be hit multiple times by the same swing
			HitActorsToIgnore.AddUnique(BoxHit.GetActor());
			MakeFieldAfterHit(BoxHit);

		}
		else 
		{
			UE_LOG(LogTemp, Warning, TEXT("Did not hit actor."));
		}
	}	
	else
	{	
		FString StateString = BuildStateString();
	}


}



bool AWeapon::CanDoDamage()
{

	//get the character that the weapon is attached to
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(GetAttachParentActor());




	if (!SlashCharacter)
	{
		return false;
	}
	else 
	{


		UE_LOG(LogTemp, Warning, TEXT("Weapon Can Do Damage Method: ActionState: %s . Weapon Collision State %s.")
			, *UEnum::GetValueAsString(SlashCharacter->GetActionState())
			, *UEnum::GetValueAsString(GetWeaponCollisionState()));

		//if the character is attacking and the animation is at a point where damage is possible
		return SlashCharacter->GetActionState() == EActionState::EAS_Attacking &&
			GetWeaponCollisionState() == EWeaponCollisionState::EWS_CollisionOn;
	}
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();
	//const FVector HalfSize = FVector(25.f); //5.f

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

	for (AActor* Actor : HitActorsToIgnore)
	{
		ActorsToIgnore.Add(Actor);
	}



	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		HalfSize,
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		BoxHit,
		true
	);
}

void AWeapon::MakeFieldAfterHit(FHitResult& BoxHit)
{
	CreateFields(BoxHit.ImpactPoint);
}


void AWeapon::PlayWeaponPickupSound()
{
	if (WeaponPickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation
		(
			this,
			WeaponPickupSound,
			GetActorLocation()
		);
	}
}



FString AWeapon::BuildStateString()
{
	FString StateString = FString();
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(GetAttachParentActor());

	if (!SlashCharacter) 
	{
		StateString = "Slash Character does not exist";
		return StateString; 
	}

	switch (SlashCharacter->GetActionState())
	{
	case EActionState::EAS_Unoccupied:
		StateString = "Unoccupied.";
		break;
	case EActionState::EAS_Equipping:
		StateString = "Equipping.";
		break;
	case EActionState::EAS_Attacking:
		StateString = "Attacking.";
		break;
	default:
		break;
	}

	switch (SlashCharacter->GetCharacterState())
	{
	case ECharacterState::ECS_Unarmed:
		StateString = StateString + " Unarmed";
		break;
	case ECharacterState::ECS_EquippedOneHanded:
		StateString = StateString + " Equipped One Handed";
		break;
	case ECharacterState::ECS_EquippedTwoHanded:
		StateString = StateString + " Equipped Two Handed";
		break;
	default:
		break;
	}

	switch (GetWeaponCollisionState())
	{
	case EWeaponCollisionState::EWS_CollisionOn:
		StateString = StateString + " Collision On";
		break;
	case EWeaponCollisionState::EWS_CollisionOff:
		StateString = StateString + " Collision Off";
		break;
	default:
		break;
	}

	return StateString;
}
