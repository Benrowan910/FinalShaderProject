#pragma once

enum class EDirection
{
	Forward, Right, Back, Left, Up, Down
};

enum class EBlock{
	Null, Air, Stone, Dirt, Grass, Moss, Sand
};

UENUM(BlueprintType)
enum class EGenerationType : uint8
{
	GT_3D UMETA(DisplayName = "3D"),
	GT_2D UMETA(DisplayName = "2D"),
};
