// Fill out your copyright notice in the Description page of Project Settings.


#include "SmoothMarchingChunk.h"

#include "MeshAttributes.h"
#include "SelectionSet.h"

ASmoothMarchingChunk::ASmoothMarchingChunk()
{
	Voxels.SetNum((Size + 1) * (Size + 1) * (Size + 1));
	
}

void ASmoothMarchingChunk::GenerateHeightMap()
{
	const auto Position = GetActorLocation() / 100;

	for(int x = 0; x<= Size; ++x)
	{
		for(int y = 0; y <= Size; ++y)
		{
			const float Xpos = x + Position.X;
			const float Ypos = y + Position.Y;

			const int Height = FMath::Clamp(FMath::RoundToInt((Noise->GetNoise(Xpos, Ypos) +1) * Size / 2), 0, Size);

			for (int z = 0; z < Height; z++)
			{
				Voxels[GetVoxelIndex(x,y,z)] = 1.0f;
			}

			for (int z = Height; z < Size; z++)
			{
				Voxels[GetVoxelIndex(x,y,z)] = -1.0f;
			}
			//for(int z = 0; z <= Size; ++z)
			//{
				//Voxels[GetVoxelIndex(x,y,z)] = Noise->GetNoise(x+ Position.X, y + Position.Y, z + Position.Z);
			//}
		}
	}
}

void ASmoothMarchingChunk::GenerateMesh()
{
	if(SurfaceLevel > 0.0f)
	{
		TriangleOrder[0] = 0;
		TriangleOrder[1] = 1;
		TriangleOrder[2] = 2;
	}
	else
	{
		TriangleOrder[0] = 2;
		TriangleOrder[1] = 1;
		TriangleOrder[2] = 0;
	}

	float Cube[8];

	for(int x = 0; x < Size; ++x)
	{
		for(int y = 0; y < Size; ++y)
		{
			for(int z = 0; z < Size; ++z)
			{
				for(int i = 0; i < 8; ++i)
				{
					Cube[i] = Voxels[GetVoxelIndex(x+VertexOffset[i][0], y + VertexOffset[i][1], z + VertexOffset[i][2])];
				}
				March(x,y,z,Cube);
			}
		}
	}
}

void ASmoothMarchingChunk::March(int X, int Y, int Z, const float Cube[8])
{
	int VertexMask = 0;
	FVector EdgeVertex[12];

	for(int i = 0; i < 8; ++i)
	{
		if(Cube[i] <= SurfaceLevel) VertexMask |= 1 << i;
	}

	const int EdgeMask = CubeEdgeFlags[VertexMask];

	if(EdgeMask == 0) return;
	

	for(int i = 0; i < 12; ++i)
	{
		if((EdgeMask & 1 << i) != 0){
			const float Offset = Interpolation ? GetInterpolationOffset(Cube[EdgeConnection[i][0]], Cube[EdgeConnection[i][1]]) : 0.5f;

			EdgeVertex[i].X = X + (VertexOffset[EdgeConnection[i][0]][0] + Offset * EdgeDirection[i][0]);
			EdgeVertex[i].Y = Y + (VertexOffset[EdgeConnection[i][0]][1] + Offset * EdgeDirection[i][1]);
			EdgeVertex[i].Z = Z + (VertexOffset[EdgeConnection[i][0]][2] + Offset * EdgeDirection[i][2]);

		}
	}

	for ( int i = 0; i < 5; ++i)
	{
		if(TriangleConnectionTable[VertexMask][3*i] < 0) break;

		auto V1 = EdgeVertex[TriangleConnectionTable[VertexMask][3 * i]] * 100;
		auto V2 = EdgeVertex[TriangleConnectionTable[VertexMask][3 * i + 1]] * 100;
		auto V3 = EdgeVertex[TriangleConnectionTable[VertexMask][3 * i + 2]] * 100;

		auto Normal = FVector::CrossProduct(V2-V1, V3-V1);
		Normal.Normalize();

		ChunkData.Vertices.Add(V1);
		ChunkData.Vertices.Add(V2);
		ChunkData.Vertices.Add(V3);

		ChunkData.Triangles.Add(VertexCount + TriangleOrder[0]);
		ChunkData.Triangles.Add(VertexCount + TriangleOrder[1]);
		ChunkData.Triangles.Add(VertexCount + TriangleOrder[2]);

		ChunkData.Normals.Add(Normal);
		ChunkData.Normals.Add(Normal);
		ChunkData.Normals.Add(Normal);


		VertexCount += 3;

	}
}

int ASmoothMarchingChunk::GetVoxelIndex(int X, int Y, int Z) const
{
	return Z * (Size + 1) * (Size + 1) + Y * (Size + 1) + X;
}

float ASmoothMarchingChunk::GetInterpolationOffset(float V1, float V2) const
{
	const float Delta = V2 - V1;
	return Delta == 0.0f ? SurfaceLevel : (SurfaceLevel - V1) / Delta;
}

