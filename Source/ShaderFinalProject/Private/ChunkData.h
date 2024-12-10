#pragma once


#include "CoreMinimal.h"
#include "ChunkData.generated.h"

USTRUCT()

struct FChunkData
{
	GENERATED_BODY();

public:
	TArray<FVector> Vertices;
	TArray<int> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FColor> Colors;
};


