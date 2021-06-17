#pragma once
#include <utility>

#define ATLAS_SIZE 2 // width and height of the texture atlas in tiles

class Block
{
public:
	enum class BlockType {
		Air,
		Grass,
		Dirt,
		Rock,
		NumberOfBlockTypes
	};

	Block(BlockType type = BlockType::Air);
	BlockType type;
	std::pair<float, float> getUV(unsigned int side);
};
