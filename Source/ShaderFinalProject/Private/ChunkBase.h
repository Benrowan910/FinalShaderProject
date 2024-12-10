// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FastNoiseLite.h"
#include "ChunkData.h"
#include "Enums.h"
#include "ProceduralMeshComponent.h"
#include "ChunkBase.generated.h"

class FastNoiseLite;
class UproceduralMeshComponent;


UCLASS()
class AChunkBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunkBase();

	UPROPERTY(EditDefaultsOnly, Category = "Chunk")
	int Size = 64;

	TObjectPtr<UMaterialInterface> Material;

	UPROPERTY(EditDefaultsOnly, Category = "HeightMap")
	float Frequency = 0.03f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GenerateHeightMap();

	virtual void GenerateMesh();

	UProceduralMeshComponent* Mesh;
	FastNoiseLite* Noise;
	FChunkData ChunkData;
	int VertexCount = 0;

public:	
	// Called every frame
	void ApplyMesh() const;

};
