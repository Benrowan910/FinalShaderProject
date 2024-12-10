// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "Enums.h"
#include "FastNoiseLite.h"
#include "FastNoiseWrapper.h"
#include "MeshUtilitiesCommon.h"
#include "ProceduralMeshComponent.h"

#include "Field/FieldSystemNoiseAlgo.h"
#include "NavMesh/NavMeshRenderingComponent.h"
#include "NavMesh/NavMeshRenderingComponent.h"

// Sets default values
AChunk::AChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh");
	Noise = new FastNoiseLite();
	Noise->SetFrequency(0.03f);
	Noise->SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);
	Noise->SetFractalType(FastNoiseLite::FractalType_DomainWarpProgressive);

	Blocks.SetNum(Size * Size * Size);

	Mesh->SetCastShadow(true);
	Mesh->HasStaticLighting();
	Mesh->SetStaticWhenNotMoveable(true);

	SetRootComponent(Mesh);
	

}

// Called when the game starts or when spawned
void AChunk::BeginPlay()
{
	Super::BeginPlay();

	GenerateBlocks();
	GenerateMesh();
	ApplyMesh();
}

void AChunk::GenerateBlocks()
{
	const auto Location = GetActorLocation();

	for (int x = 0; x < Size; ++x)
	{
		for(int y = 0; y < Size; ++y)
		{
			const float Xpos = (x * 100 + Location.X) / 100;
			const float Ypos = (y * 100 + Location.Y) / 100;

			const int Height = FMath::Clamp(FMath::RoundToInt((Noise->GetNoise(Xpos, Ypos) + 1) * Size / 2),0,Size);

			for(int z = 0; z < Size; z++)
			{
				if (z < Height - 4) Blocks[GetBlockIndex(x, y, z)] = EBlock::Sand;
				else if(z < Height - 3) Blocks[GetBlockIndex(x,y,z)] = EBlock::Stone;
				else if (z < Height - 1) Blocks[GetBlockIndex(x, y, z)] = EBlock::Moss;
				else if (z == Height - 1) Blocks[GetBlockIndex(x, y, z)] = EBlock::Grass;
				else Blocks[GetBlockIndex(x, y, z)] = EBlock::Air;
			}
		}
	}
}

void AChunk::ApplyMesh()
{
	Mesh->SetMaterial(0, Material);
	Mesh->CreateMeshSection(0, ChunkData.Vertices, ChunkData.Triangles, ChunkData.Normals, ChunkData.UV0, ChunkData.Colors, TArray<FProcMeshTangent>(), true);
}

void AChunk::GenerateMesh()
{
	for(int Axis = 0; Axis < 3; ++Axis)
	{
		const int Axis1 = (Axis + 1 ) % 3;
		const int Axis2 = (Axis + 2 ) % 3;

		const int MainAxisLimit = Size;
		int Axis1Limit = Size;
		int Axis2Limit = Size;

		auto DeltaAxis1 = FIntVector::ZeroValue;
		auto DeltaAxis2 = FIntVector::ZeroValue;

		auto ChunkItr = FIntVector::ZeroValue;
		auto AxisMask = FIntVector::ZeroValue;

		AxisMask[Axis] = 1;

		TArray<FMask> Mask;
		Mask.SetNum(Axis1Limit * Axis2Limit);

		for(ChunkItr[Axis] = -1; ChunkItr[Axis] < MainAxisLimit;)
		{
			int N = 0;

			for(ChunkItr[Axis2] = 0; ChunkItr[Axis2] < Axis2Limit; ++ChunkItr[Axis2])
			{
				for(ChunkItr[Axis1] = 0; ChunkItr[Axis1] < Axis1Limit; ++ChunkItr[Axis1])
				{
					const auto CurrentBlock = GetBlock(ChunkItr);
					const auto CompareBlock = GetBlock(ChunkItr + AxisMask);
					
					const bool CurrentBlockOpaque = CurrentBlock != EBlock::Air;
					const bool CompareBlockOpaque = CompareBlock != EBlock::Air;

					if(CurrentBlockOpaque == CompareBlockOpaque)
					{
						Mask[N++] = FMask {EBlock::Null, 0};
						
					}	else if(CurrentBlockOpaque)
					{
						Mask[N++] = FMask {CurrentBlock, 1};
					}
					else
					{
						Mask[N++] = FMask {CompareBlock, -1};
					}
				}
			}
			++ChunkItr[Axis];
			N= 0;

			//Generate Mesh from a mask that is being generated.
			for(int j = 0; j < Axis2Limit; ++j)
			{
				for(int i = 0; i < Axis1Limit;)
				{
					if(Mask[N].Normal != 0)
					{
						const auto CurrentMask = Mask[N];

						ChunkItr[Axis1] = i;
						ChunkItr[Axis2] = j;

						int width;

						for(width = 1; i + width < Axis1Limit && CompareMask(Mask[N + width], CurrentMask); ++ width)
						{
							
						}

						int height;
						bool done = false;

						for(height = 1; j + height < Axis2Limit; ++height)
						{
							for(int k = 0; k < width; ++k)
							{
								if(CompareMask(Mask[N+k+height * Axis1Limit], CurrentMask)) continue;

								done = true;
								break;
							}

							if(done) break;
						}

						DeltaAxis1[Axis1] = width;
						DeltaAxis2[Axis2] = height;

						//Create quads now

						CreateQuad(CurrentMask, AxisMask, width, height, ChunkItr, ChunkItr + DeltaAxis1, ChunkItr + DeltaAxis2, ChunkItr + DeltaAxis1 + DeltaAxis2);

						//Clean the axis data.
						DeltaAxis1 = FIntVector::ZeroValue;
						DeltaAxis2 = FIntVector::ZeroValue;

						for(int l = 0; l < height; ++l)
						{
							for(int k = 0; k < width; ++k)
							{
								Mask[N+k+l * Axis1Limit] = FMask{EBlock::Null, 0};
							}
						}

						i += width;
						N += width;
					}else
					{
						i++;
						N++;
					}
				}
			}
		}
	}
}

void AChunk::CreateQuad(FMask Mask, FIntVector AxisMask, const int width, const int height, FIntVector v1, FIntVector v2, FIntVector v3, FIntVector v4)
{
	const auto Normal = FVector(AxisMask * Mask.Normal);
	const auto Color = FVector(0, 93, 93);

	ChunkData.Vertices.Add(FVector(v1) * 100);
	ChunkData.Vertices.Add(FVector(v2) * 100);
	ChunkData.Vertices.Add(FVector(v3) * 100);
	ChunkData.Vertices.Add(FVector(v4) * 100);

	ChunkData.Triangles.Add(VertexCount);
	ChunkData.Triangles.Add(VertexCount + 2 + Mask.Normal);
	ChunkData.Triangles.Add(VertexCount + 2 - Mask.Normal);
	ChunkData.Triangles.Add(VertexCount + 3);
	ChunkData.Triangles.Add(VertexCount + 1 - Mask.Normal);
	ChunkData.Triangles.Add(VertexCount + 1 + Mask.Normal);

	if(Normal.X==1 || Normal.X == -1)
	{
		ChunkData.UV0.Append({
			(FVector2D(width,height)),
			(FVector2D(0,height)),
			(FVector2D(width,0)),
			(FVector2D(0,0))});
	}
	else
	{
		ChunkData.UV0.Append({
			(FVector2D(height,width)),
			(FVector2D(height,0)),
			(FVector2D(0,width)),
			(FVector2D(0,0))});
	}


	ChunkData.Normals.Add(Normal);
	ChunkData.Normals.Add(Normal);
	ChunkData.Normals.Add(Normal);
	ChunkData.Normals.Add(Normal);

	const auto color = FColor(0, 0,0,GetTextureIndex(Mask.Block, Normal));

	ChunkData.Colors.Append({
	color, color, color, color});

	VertexCount += 4;
}

int AChunk::GetBlockIndex(int X, int Y, int Z) const
{
	return Z * Size * Size + Y * Size + X;
}

EBlock AChunk::GetBlock(FIntVector Index) const
{
	if (Index.X >= Size || Index.Y >= Size || Index.Z >= Size || Index.X < 0 || Index.Y < 0 || Index.Z < 0)
		return EBlock::Air;
	return Blocks[GetBlockIndex(Index.X, Index.Y, Index.Z)];
}

bool AChunk::CompareMask(FMask M1, FMask M2) const
{
	return M1.Block == M2.Block && M1.Normal == M2.Normal;
}

int AChunk::GetTextureIndex(EBlock Block, FVector Normal) const
{
	switch(Block)
	{
	case EBlock::Grass:
		{
			if(Normal == FVector::UpVector) return 0;
			if(Normal == FVector::DownVector) return 1;
			return 4;
		}
	case EBlock::Moss:
		{
			if(Normal == FVector::UpVector) return 1;
			if(Normal == FVector::DownVector) return 1;
			if(Normal == FVector::RightVector) return 1;
			if(Normal == FVector::LeftVector) return 1;
			if(Normal == FVector::ForwardVector) return 1;
			if(Normal == FVector::BackwardVector) return 1;

			return 1;
		}
	case EBlock::Stone:
		{
			if(Normal == FVector::UpVector) return 2;
			if(Normal == FVector::DownVector) return 2;
			if(Normal == FVector::RightVector) return 2;
			if(Normal == FVector::LeftVector) return 2;
			if(Normal == FVector::ForwardVector) return 2;
			if(Normal == FVector::BackwardVector) return 2;

			return 2;
		}
	case EBlock::Sand:
		{
			if(Normal == FVector::UpVector) return 3;
			if(Normal == FVector::DownVector) return 3;
			if(Normal == FVector::RightVector) return 3;
			if(Normal == FVector::LeftVector) return 3;
			if(Normal == FVector::ForwardVector) return 3;
			if(Normal == FVector::BackwardVector) return 3;

			return 3;
		}
	default: return 255;
	}
}

