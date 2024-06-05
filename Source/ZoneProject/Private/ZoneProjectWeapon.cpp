// Copyright Anton Romanov. All Rights Reserved.

#include "ZoneProjectWeapon.h"
#include "ZoneProjectCharacter.h"

AZoneProjectWeapon::AZoneProjectWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	// Set up the mesh component

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

void AZoneProjectWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AZoneProjectWeapon::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	if (HasAuthority())
	{
		Character = Cast<AZoneProjectCharacter>(GetInstigator());
		if (Character) AddTickPrerequisiteActor(Character);
	}
}

void AZoneProjectWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AZoneProjectWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AZoneProjectWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AZoneProjectWeapon::OnRep_Instigator()
{
	Super::OnRep_Instigator();

	Character = Cast<AZoneProjectCharacter>(GetInstigator());
	if (Character) AddTickPrerequisiteActor(Character);
}

void AZoneProjectWeapon::ReplicateFire(const FRotator Rotation)
{
	if (Character)
	{
		if (Character->GetLocalRole() == ROLE_AutonomousProxy)
		{
			Character->ServerFire();
		}
		else if (Character->GetLocalRole() == ROLE_Authority)
		{
			Character->MulticastFire(Rotation);
		}
	}
}
