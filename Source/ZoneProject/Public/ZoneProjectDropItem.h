// Copyright Anton Romanov. All Rights Reserved.

#pragma once

#include "ZoneProject/ZoneProject.h"
#include "ZoneProjectTypes.h"
#include "GameFramework/Actor.h"
#include "ZoneProjectDropItem.generated.h"

/**
 * Drop Item class
 */
UCLASS(Blueprintable)
class ZONEPROJECT_API AZoneProjectDropItem : public AActor
{
	GENERATED_BODY()
	
public:	

	/* Class constructor */
	AZoneProjectDropItem();

protected:

	/* Called when the game starts or when spawned */
	virtual void BeginPlay() override;

public:

	/* Called every frame */
	virtual void Tick(float DeltaTime) override;

public:

	/* Type of the drop item */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "General")
	EDropItemType Type = EDropItemType::None;

	/* Value applied to the character depending on the @Type */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "General")
	float Amount = 0.f;

	/* Called when this actor begins to overlap with another actor */
	UFUNCTION() void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
};
