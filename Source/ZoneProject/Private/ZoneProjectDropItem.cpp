// Copyright Anton Romanov. All Rights Reserved.

#include "ZoneProjectDropItem.h"
#include "ZoneProjectCharacter.h"

AZoneProjectDropItem::AZoneProjectDropItem()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AZoneProjectDropItem::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OnActorBeginOverlap.AddDynamic(this, &AZoneProjectDropItem::OnBeginOverlap);
	}
}

void AZoneProjectDropItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AZoneProjectDropItem::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (AZoneProjectCharacter* Character = Cast<AZoneProjectCharacter>(OtherActor))
	{
		if (Character->IsPlayerControlled())
		{
			if (Type == EDropItemType::Health)
			{
				Character->AddHealth(Amount);
				Destroy();
			}
		}
	}
}
