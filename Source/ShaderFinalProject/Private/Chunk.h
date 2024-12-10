// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "FastNoiseLite.h"
#include "ChunkData.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Chunk.generated.h"

enum class EBlock;
enum class EDirection;
class FastNoiseLite;
class UProceduralMeshComponent;

UCLASS()
class SHADERFINALPROJECT_API AChunk : public AActor
{
	GENERATED_BODY()

	struct FMask{
		EBlock Block;
		int Normal;
	};

	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material;
	
public:	
	// Sets default values for this actor's properties
	AChunk();

	UPROPERTY(EditAnywhere, Category = "Chunk")
	//FIntVector Size = FIntVector(1,1,1) * 32;
	int Size = 64;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	

	UProceduralMeshComponent* Mesh;//TObjectPtr<UProceduralMeshComponent> Mesh;

	FastNoiseLite* Noise;//TObjectPtr<FastNoiseLite> Noise;

	FChunkData ChunkData;

	TArray<EBlock> Blocks;
	
	int VertexCount = 0;

	void GenerateBlocks();

	void ApplyMesh();
	
	void GenerateMesh();

	void CreateQuad(FMask Mask, FIntVector AxisMask, const int width, const int height, FIntVector v1, FIntVector v2, FIntVector v3, FIntVector v4);

	int GetBlockIndex(int X, int Y, int Z) const;

	EBlock GetBlock(FIntVector Index) const;

	bool CompareMask(FMask M1, FMask M2) const;

	int GetTextureIndex(EBlock Block, FVector Normal) const;
	
};
