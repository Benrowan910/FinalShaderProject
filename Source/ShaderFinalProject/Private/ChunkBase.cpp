// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkBase.h"

// Sets default values
AChunkBase::AChunkBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh");
	Noise = new FastNoiseLite();
Noise->SetFrequency(Frequency);
	Noise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	Noise->SetFractalType(FastNoiseLite::FractalType_FBm);

	Mesh->SetCastShadow(false);

	SetRootComponent(Mesh);
	
}

// Called when the game starts or when spawned
void AChunkBase::BeginPlay()
{
	Super::BeginPlay();

	GenerateHeightMap();
	GenerateMesh();
	
}

void AChunkBase::GenerateHeightMap()
{
}

void AChunkBase::GenerateMesh()
{
}

void AChunkBase::ApplyMesh() const
{
	Mesh->CreateMeshSection(0, ChunkData.Vertices, ChunkData.Triangles, ChunkData.Normals, ChunkData.UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
}



