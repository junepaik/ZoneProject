// Copyright Anton Romanov. All Rights Reserved.

#include "ZoneProjectCharacter.h"
#include "ZoneProjectCharacterMovement.h"
#include "ZoneProjectDropItem.h"
#include "ZoneProjectWeapon.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Materials/Material.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"

AZoneProjectCharacter::AZoneProjectCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UZoneProjectCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create the camera boom component
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 2000.f;
	CameraBoom->SetRelativeRotation(FRotator(-55.f, -90.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create the main camera component
	
	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	MainCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	MainCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AZoneProjectCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AZoneProjectCharacter, MaxHealth);
	DOREPLIFETIME(AZoneProjectCharacter, Health);
	DOREPLIFETIME(AZoneProjectCharacter, Weapon);
}

void AZoneProjectCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AZoneProjectCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HasAuthority())
	{
		if (DefaultWeaponClass)
		{
			FActorSpawnParameters SpawnInfo;

			SpawnInfo.Owner = this;
			SpawnInfo.Instigator = this;
			
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			
			Weapon = GetWorld()->SpawnActor<AZoneProjectWeapon>(DefaultWeaponClass, GetActorTransform(), SpawnInfo);
			
			if (Weapon)
			{
				const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
				Weapon->AttachToComponent(GetMesh(), AttachmentRules, WeaponSocketName);
			}
		}
	}
}

void AZoneProjectCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AZoneProjectCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if  (HasAuthority())
	{
		if (Weapon) Weapon->Destroy();
	}
	
	Super::EndPlay(EndPlayReason);
}

void AZoneProjectCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

float AZoneProjectCharacter::InternalTakePointDamage(float Damage, FPointDamageEvent const& PointDamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	SetHealth(Health - Damage);
	
	if (Health == 0.f) MulticastDeath();
	
	return Damage;
}

void AZoneProjectCharacter::SpawnDropItem()
{
	for (auto& DropItemProbability : DropItemProbabilities)
	{
		if (FMath::FRand() < DropItemProbability.Value)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			
			GetWorld()->SpawnActor<AZoneProjectDropItem>(DropItemProbability.ItemClass, GetActorTransform(), SpawnInfo);
			return;
		}
	}
}

void AZoneProjectCharacter::InternalOnDeath()
{
	bIsAlive = false;
	Health = 0.f;
	
	GetCapsuleComponent()->SetCollisionProfileName(FName(TEXT("NoCollision")));
		
	// Enable ragdoll
	GetMesh()->SetCollisionProfileName(FName(TEXT("Ragdoll")));
	GetMesh()->SetAllBodiesSimulatePhysics(true);

	if (HasAuthority())
	{
		SpawnDropItem();
		
		// Remove the character after a delay
		
		FTimerManager& TimerManager = GetWorldTimerManager();
		TimerManager.SetTimer(RemoveTimer, this, &AZoneProjectCharacter::RemoveCharacter, 10.f, false);
	}
}

void AZoneProjectCharacter::RemoveCharacter()
{
	Destroy();
}

FRotator AZoneProjectCharacter::GetBaseAimRotation() const
{
	if (GetLocalRole() >= ROLE_AutonomousProxy)
	{
		return GetControlRotation();
	}
	else
	{
		FRotator BaseAimRotation = GetActorRotation();
		
		BaseAimRotation.Pitch = FRotator::NormalizeAxis(RemoteViewPitch * 360.0f / 255.0f);
		BaseAimRotation.Yaw   = FRotator::NormalizeAxis(BaseAimRotation.Yaw);
		BaseAimRotation.Roll  = FRotator::NormalizeAxis(BaseAimRotation.Roll);

		return BaseAimRotation;
	}
}

bool AZoneProjectCharacter::CanSprint() const
{
	if (const UZoneProjectCharacterMovement* CharacterMovementCasted = Cast<UZoneProjectCharacterMovement>(GetCharacterMovement()))
	{
		return CharacterMovementCasted->bCanSprint && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
	}

	return false;
}

void AZoneProjectCharacter::Sprint()
{
	if (CanSprint())
	{
		bIsSprinting = true;

		if (UZoneProjectCharacterMovement* CharacterMovementCasted = Cast<UZoneProjectCharacterMovement>(GetCharacterMovement()))
		{
			CharacterMovementCasted->bOrientRotationToMovement = true;
			if (IsLocallyControlled()) CharacterMovementCasted->bWantsToSprint = true;
		}
	}
}

void AZoneProjectCharacter::UnSprint()
{
	bIsSprinting = false;

	if (UZoneProjectCharacterMovement* CharacterMovementCasted = Cast<UZoneProjectCharacterMovement>(GetCharacterMovement()))
	{
		CharacterMovementCasted->bOrientRotationToMovement = false;
		if (IsLocallyControlled()) CharacterMovementCasted->bWantsToSprint = false;
	}
}

void AZoneProjectCharacter::StartFire()
{
	if (!bIsSprinting)
	{
		if (Weapon) Weapon->StartFire();
	}
}

void AZoneProjectCharacter::StopFire()
{
	if (Weapon) Weapon->StopFire();
}

void AZoneProjectCharacter::SimulateFire(const FRotator Rotation)
{
	if (Weapon) Weapon->SimulateFire(Rotation);
}

void AZoneProjectCharacter::MulticastDeath_Implementation()
{
	InternalOnDeath();
}

void AZoneProjectCharacter::ServerFire_Implementation()
{
	StartFire();
}

void AZoneProjectCharacter::MulticastFire_Implementation(const FRotator Rotation)
{
	if (GetLocalRole() == ROLE_SimulatedProxy) SimulateFire(Rotation);
}
