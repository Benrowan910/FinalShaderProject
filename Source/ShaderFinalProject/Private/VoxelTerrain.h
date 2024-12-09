// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelTerrain.generated.h"

UCLASS()
class AVoxelTerrain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVoxelTerrain();

	UPROPERTY(EditAnywhere, Category = "Voxel Terrain")
	TSubclassOf<AActor> Chunk;

	UPROPERTY(EditAnywhere, Category = "Voxel Terrain")
	int DrawDistance = 5;

	UPROPERTY(EditAnywhere, Category = "Voxel Terrain")
	int ChunkSize = 32;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
