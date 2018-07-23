// Fill out your copyright notice in the Description page of Project Settings.

#include "S05_TestingGrounds.h"
#include "Tile.h"
#include "DrawDebugHelpers.h"
#include "ActorPool.h"


// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MinExtent = FVector(0, -2000, 0);
	MaxExtent = FVector(4000, 2000, 0);
	NavigationBoundsOffset = FVector(2000, 0, 0);
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

}

void ATile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Pool->Return(NavMeshBoundsVolume);
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATile::SetPool(UActorPool * InPool)
{
	Pool = InPool;

	PositionNavMeshBoundsVolume();
}
void ATile::PositionNavMeshBoundsVolume()
{
	NavMeshBoundsVolume = Pool->Checkout();
	if (NavMeshBoundsVolume == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Not enough actors in pool."), *GetName());
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("[%s] Checked Out {%s}."), *GetName(), *NavMeshBoundsVolume->GetName());
	NavMeshBoundsVolume->SetActorLocation(GetActorLocation()+ NavigationBoundsOffset);
	GetWorld()->GetNavigationSystem()->Build();
}

bool ATile::FindEmptyLocation(FVector& OutLocation, float Radius)
{
	FBox Bounds(MinExtent, MaxExtent);
	const int MAX_ATTEMPTS = 100;

	for (size_t i = 0; i < MAX_ATTEMPTS; i++)
	{
		FVector CandidatePoint = FMath::RandPointInBox(Bounds);

		if (CanSpawnAtLocation(CandidatePoint, Radius))
		{
			OutLocation = CandidatePoint;
			return true;
		}
	}
	return false;
}

void ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, FSpawnPosition& SpawnPosition)
{
	AActor* Spawned = GetWorld()->SpawnActor(ToSpawn);
	if (Spawned)
	{
		Spawned->SetActorRelativeLocation(SpawnPosition.Location);
		Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		Spawned->SetActorRotation(FRotator(0, SpawnPosition.Rotation, 0));
		Spawned->SetActorScale3D(FVector(SpawnPosition.Scale));
	}
}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, FSpawnProperties& SpawnProperties)
{
	RandomlyPlaceActors(ToSpawn, SpawnProperties);
}

void ATile::PlaceAIPawns(TSubclassOf<APawn> ToSpawn, FSpawnProperties& SpawnProperties)
{
	RandomlyPlaceActors(ToSpawn, SpawnProperties);
}

void ATile::PlaceActor(TSubclassOf<APawn> ToSpawn, FSpawnPosition& SpawnPosition)
{
	APawn* Spawned = GetWorld()->SpawnActor<APawn>(ToSpawn);
	if (Spawned)
	{
		Spawned->SetActorRelativeLocation(SpawnPosition.Location);
		Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		Spawned->SetActorRotation(FRotator(0, SpawnPosition.Rotation, 0));
		//Spawned->SetActorScale3D(FVector(SpawnPosition.Scale)); Scaling options
		Spawned->SpawnDefaultController();
		Spawned->Tags.Add(FName("Enemy"));
	}
}

template <class T>
void ATile::RandomlyPlaceActors(TSubclassOf<T> ToSpawn, FSpawnProperties& SpawnProperties)
{
	TArray<FSpawnPosition> SpawnPositions;
	int NumberToSpawn = FMath::RandRange(SpawnProperties.MinSpawn, SpawnProperties.MaxSpawn);
	for (size_t i = 0; i < NumberToSpawn; i++)
	{
		FSpawnPosition SpawnPosition;
		SpawnPosition.Scale = FMath::RandRange(SpawnProperties.MinScale, SpawnProperties.MaxScale);
		bool found = FindEmptyLocation(SpawnPosition.Location, SpawnProperties.Radius * SpawnPosition.Scale);
		if (found) {
			SpawnPosition.Rotation = FMath::RandRange(-180.f, 180.f);
			PlaceActor(ToSpawn, SpawnPosition);
		}
	}
}

TArray<FSpawnPosition> ATile::RandomSpawnPositions(FSpawnProperties SpawnProperties)
{
	TArray<FSpawnPosition> SpawnPositions;
	int NumberToSpawn = FMath::RandRange(SpawnProperties.MinSpawn, SpawnProperties.MaxSpawn);
	for (size_t i = 0; i < NumberToSpawn; i++)
	{
		FSpawnPosition SpawnPosition;
		SpawnPosition.Scale = FMath::RandRange(SpawnProperties.MinScale, SpawnProperties.MaxScale);
		bool found = FindEmptyLocation(SpawnPosition.Location, SpawnProperties.Radius * SpawnPosition.Scale);
		if (found) {
			SpawnPosition.Rotation = FMath::RandRange(-180.f, 180.f);
			SpawnPositions.Add(SpawnPosition);
		}
	}

	return SpawnPositions;
}

bool ATile::CanSpawnAtLocation(FVector Location, float Radius)
{
	FHitResult HitResult;
	FVector GlobalLocation = ActorToWorld().TransformPosition(Location);
	bool HasHit = GetWorld()->SweepSingleByChannel(HitResult, GlobalLocation, GlobalLocation, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel2, FCollisionShape::MakeSphere(Radius));

	// FColor ResultColour = HasHit ? FColor::Red : FColor::Green;
	//DrawDebugCapsule(GetWorld(), GlobalLocation, 0, Radius, FQuat::Identity, ResultColour, true, 100);
	return !HasHit;
}





