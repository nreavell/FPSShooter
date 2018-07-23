// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

USTRUCT()
struct FSpawnPosition
{
	GENERATED_USTRUCT_BODY()

	FVector Location;
	float Rotation;
	float Scale;
};

USTRUCT(BlueprintType)
struct FSpawnProperties
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SpawnProperties")
	int MinSpawn = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SpawnProperties")
	int MaxSpawn = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SpawnProperties")
	float Radius = 500.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SpawnProperties")
	float MinScale = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SpawnProperties")
	float MaxScale = 1.f;
};


class UActorPool;

UCLASS()
class S05_TESTINGGROUNDS_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	FVector MinExtent;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	FVector MaxExtent;

	UPROPERTY(EditDefaultsOnly, Category = "Navigation")
	FVector NavigationBoundsOffset;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void PlaceActors(TSubclassOf<AActor> ToSpawn, UPARAM(ref) FSpawnProperties& SpawnProperties);

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void PlaceAIPawns(TSubclassOf<APawn> ToSpawn, UPARAM(ref) FSpawnProperties& SpawnProperties);

	UFUNCTION(BlueprintCallable, Category = "Pool")
	void SetPool(UActorPool* InPool);

private:

	bool CanSpawnAtLocation(FVector Location, float Radius);

	bool FindEmptyLocation(FVector& OutLocation, float Radius);

	void PlaceActor(TSubclassOf<AActor> ToSpawn, FSpawnPosition& SpawnPosition);

	void PlaceAIPawn(TSubclassOf<APawn> ToSpawn, FSpawnPosition& SpawnPosition);

	TArray<FSpawnPosition> RandomSpawnPositions(FSpawnProperties SpawnProperties);

	void PositionNavMeshBoundsVolume();

	UActorPool* Pool;

	AActor* NavMeshBoundsVolume;
};
