// Copyright Anton Romanov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZoneProjectTypes.h"
#include "GameFramework/GameModeBase.h"
#include "ZoneProjectGameMode.generated.h"

/**
 * Game Mode class
 */
UCLASS(MinimalAPI)
class AZoneProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	/* Class constructor */
	AZoneProjectGameMode();

protected:

	/* Called when the game starts or when spawned */
	virtual void BeginPlay() override;

public:

	/* Default enemy class */
	UPROPERTY(Category = "Classes", EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AZoneProjectCharacter> DefaultEnemyClass;

	/* Frequency of spawning a new enemy */
	UPROPERTY(Category = "Game", BlueprintReadOnly, EditDefaultsOnly)
	float EnemySpawnRate = 5.f;

	/* Frequency of spawning a new enemy */
	UPROPERTY(Category = "Game", BlueprintReadOnly, EditDefaultsOnly)
	float EnemySpawnDistance = 1500.f;

protected:

	/* Timer handle for spawning enemies */
	FTimerHandle RemoveTimer;

protected:

	/* Spawn an enemy character */
	UFUNCTION() virtual void SpawnEnemy();
};
