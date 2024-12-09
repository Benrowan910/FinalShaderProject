// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"
#include "Enums.h"
#include "FastNoiseLite.h"
#include "FastNoiseWrapper.h"
#include "ProceduralMeshComponent.h"

#include "Field/FieldSystemNoiseAlgo.h"

// Sets default values
AChunk::AChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh");
	Noise = new FastNoiseLite();
	Noise->SetFrequency(0.03f);
	Noise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	Noise->SetFractalType(FastNoiseLite::FractalType_FBm);

	Blocks.SetNum(Size * Size * Size);

	Mesh->SetCastShadow(false);

}

// Called when the game starts or when spawned
void AChunk::BeginPlay()
{
	Super::BeginPlay();

	GenerateBlocks();
	GenerateMesh();
	ApplyMesh();
}

// Called every frame
void AChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChunk::GenerateBlocks()
{
	const auto Location = GetActorLocation();

	for (int x = 0; x < Size; ++x)
	{
		for (int y = 0; y < Size; ++y)
		{
			const float Xpos = (x * 100 + Location.X) / 100;
			const float Ypos = (y * 100 + Location.Y) / 100;

			const int Height = FMath::Clamp(FMath::RoundToInt((Noise->GetNoise(Xpos, Ypos) +1) * Size / 2), 0, Size);

			for (int z = 0; z < Height; ++z)
			{
				Blocks[GetBlockIndex(x,y,z)] = EBlock::Grass;
			}

			for (int z = 0; z < Height / 2; ++z)
			{
				Blocks[GetBlockIndex(x,y,z)] = EBlock::Moss;
			}

			for (int z = Height; z < Size; ++z)
			{
				Blocks[GetBlockIndex(x,y,z)] = EBlock::Air;
			}
		}
	}
}

void AChunk::GenerateMesh()
{
	for( int x = 0; x < Size; ++x)
	{
		for(int y = 0; y < Size; ++y)
		{
			for(int z = 0; z < Size; ++z)
			{
				if(Blocks[GetBlockIndex(x,y,z)] != EBlock::Air)
				{
					const auto Position = FVector(x,y,z);

					for(auto Direction : {EDirection::Forward, EDirection::Right, EDirection::Back, EDirection::Left, EDirection::Up, EDirection::Down})
					{
						if (Check(GetPositionDirection(Direction, Position)))
						{
							CreateFace(Direction, Position * 100);
						}
					}
				}
			}
		}
	}
}

void AChunk::ApplyMesh() const
{
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(SandMaterial, Mesh);

    Mesh->SetMaterial(0, DynamicMaterial);
	Mesh->CreateMeshSection(0, VertexData, TriangleData, Normals, UVData, Colors, TArray<FProcMeshTangent>(), true);
	//FBox CollisionBox(VertexData);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECC_WorldStatic);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);
	//Mesh->SetCollisionEnabled(ECC_Complex);
	Mesh->UpdateCollisionProfile();

	for(int i = 0; i < BlockTextures.Num(); i++)
	{
		UTexture2D* CurrentTexture = BlockTextures[i];

		DynamicMaterial->SetTextureParameterValue(FName(TEXT("BlockTexture")), CurrentTexture);
	}
	
	
}

bool AChunk::Check(FVector Position) const
{
	if (Position.X >= Size || Position.Y >= Size || Position.Z >= Size || Position.X < 0 || Position.Y < 0 || Position.Z < 0)
	{
		return true;
	}

	return Blocks[GetBlockIndex(Position.X, Position.Y, Position.Z)] ==EBlock::Air;
}

void AChunk::CreateFace(EDirection Direction, FVector Position)
{
	UTexture2D* BlockTexture = GetBlockTexture(Position.Z);
	VertexData.Append(GetFaceVertices(Direction, Position));
	UVData.Append({FVector2D(1,1), FVector2D(1,0), FVector2D(0,0), FVector2D(0,1)});
	TriangleData.Append({VertexCount + 3, VertexCount + 2, VertexCount, VertexCount + 2, VertexCount + 1, VertexCount});
	const auto Color = FColor(96,35,115,255);
    Colors.Append({
    Color, Color, Color, Color});

	BlockTextures.Add(BlockTexture);
	VertexCount += 4;
}

TArray<FVector> AChunk::GetFaceVertices(EDirection Direction, FVector Position) const
{
	TArray<FVector> Vertices;

	for (int i = 0; i < 4; ++i)
	{
		Vertices.Add(BlockVertexData[BlockTriangleData[i + static_cast<int>(Direction) * 4]] * Scale + Position);
	}

	return Vertices;
}

FVector AChunk::GetPositionDirection(EDirection Direction, FVector Position) const
{
	switch (Direction)
	{
	case EDirection::Forward: return Position + FVector::ForwardVector;
	case EDirection::Right: return Position + FVector::RightVector;
	case EDirection::Back: return Position + FVector::BackwardVector;
	case EDirection::Left: return Position + FVector::LeftVector;
	case EDirection::Up: return Position + FVector::UpVector;
	case EDirection::Down: return Position + FVector::DownVector;
	default: throw std::invalid_argument("Invalid Direction");
	}
}

int AChunk::GetBlockIndex(int X, int Y, int Z) const
{
	return Z * Size * Size + Y * Size + X;
}

UTexture2D* AChunk::GetBlockTexture(int BlockHeight) const
{
	if (BlockHeight < Size / 4)  // For blocks at the bottom
	{
		return GrassTex;
	}
	else if (BlockHeight < Size / 2)  // Middle layer, maybe dirt
	{
		return MossTex;
	}
	else  // Higher layers, stone
	{
		return SandTex;
	}
}

int AChunk::GetTextureIndex(EBlock Block, FVector Normal) const
{
	switch (Block)
	{
	case EBlock::Grass:
		{
			return 0;
		}
	case EBlock::Moss:
		{
			return 1;
		}
	case EBlock::Stone:
		{
			return 2;
		}
	case EBlock::Sand:
		{
			return 3;
		}
	default: return NULL;
	}
}

