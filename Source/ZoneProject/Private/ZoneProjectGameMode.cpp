// Copyright Anton Romanov. All Rights Reserved.

#include "ZoneProjectGameMode.h"
#include "ZoneProjectCharacter.h"
#include "Kismet/GameplayStatics.h"

AZoneProjectGameMode::AZoneProjectGameMode()
{
}

void AZoneProjectGameMode::BeginPlay()
{
	Super::BeginPlay();

	FTimerManager& TimerManager = GetWorldTimerManager();
	TimerManager.SetTimer(RemoveTimer, this, &AZoneProjectGameMode::SpawnEnemy, EnemySpawnRate, true);
}

void AZoneProjectGameMode::SpawnEnemy()
{
	if (DefaultEnemyClass)
	{
		if (const APlayerController* Player = UGameplayStatics::GetPlayerController(this, 0))
		{
			if (const APawn* PlayerPawn = Player->GetPawn())
			{
				// Calculate the enemy spawn position
				
				FVector Origin = PlayerPawn->GetActorLocation();

				const float Angle = FMath::RandRange(-180.f, 180.f);

				const float X = EnemySpawnDistance * FMath::Cos(Angle);
				const float Y = EnemySpawnDistance * FMath::Sin(Angle);

				Origin.X += X;
				Origin.Y += Y;

				// Spawn the enemy

				const FTransform SpawnTransform(Origin);

				FActorSpawnParameters SpawnInfo;
				SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				
				GetWorld()->SpawnActor<AZoneProjectCharacter>(DefaultEnemyClass, SpawnTransform, SpawnInfo);
			}
		}
	}
}
