// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "FastNoiseLite.h"
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
	
public:	
	// Sets default values for this actor's properties
	AChunk();

	UPROPERTY(EditAnywhere, Category = "Chunk")
	int Size = 32;

	UPROPERTY(EditAnywhere, Category = "Chunk")
	int Scale = 1;

	UPROPERTY(EditAnywhere)
	UTexture2D* GrassTex;
	
	UPROPERTY(EditAnywhere)
	UTexture2D* MossTex;
	
	UPROPERTY(EditAnywhere)
	UTexture2D* RockTex;
	
	UPROPERTY(EditAnywhere)
	UTexture2D* SandTex;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UProceduralMeshComponent* Mesh;//TObjectPtr<UProceduralMeshComponent> Mesh;

	FastNoiseLite* Noise;//TObjectPtr<FastNoiseLite> Noise;

	TArray<EBlock> Blocks;

	TArray<FVector> VertexData;

	TArray<int> TriangleData;

	TArray<FVector2D> UVData;

	int VertexCount = 0;

	TArray<UTexture2D*> BlockTextures;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* SandMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* GrassMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* RockMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* MossMaterial;
	
	const FVector BlockVertexData[8] = {
	FVector(100, 100, 100),
	FVector(100,0,100),
	FVector(100,0,0),
	FVector(100,100,0),
	FVector(0,0,100),
	FVector(0,100,100),
	FVector(0,100,0),
	FVector(0,0,0)
	};

	const int BlockTriangleData[24] = {
	0,1,2,3, //Forward
	5,0,3,6, //Right
	4,5,6,7, //Back
	1,4,7,2, //Left
	5,4,1,0, //Up
	3,2,7,6 //Down
	};

	void GenerateBlocks();

	void GenerateMesh();

	void ApplyMesh() const;

	bool Check(FVector Position) const;

	void CreateFace(EDirection Direction, FVector Position);

	TArray<FVector> GetFaceVertices(EDirection Direction, FVector Position) const;

	FVector GetPositionDirection(EDirection Direction, FVector Position) const;

	int GetBlockIndex(int X, int Y, int Z) const;

	UTexture2D* GetBlockTexture(int BlockHeight) const;

	int GetTextureIndex(EBlock Block, FVector Normal) const;

	
};
