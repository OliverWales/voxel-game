#include "Block.h"

Block::Block(BlockType type) : type(type) {}

// Returns the top left corner of the corresponding tile in the texture atlas
std::pair<float, float> Block::getUV(unsigned int side)
{
	switch (type)
	{
	case Block::BlockType::Grass:
		if(side == 5) // top
			return std::pair<float, float>(0.0f, 0.0f);
		else if (side == 2) // bottom
			return std::pair<float, float>(1.0f / ATLAS_SIZE, 1.0f / ATLAS_SIZE);
		else
			return std::pair<float, float>(1.0f / ATLAS_SIZE, 0.0f);
	case Block::BlockType::Dirt:
		return std::pair<float, float>(1.0f / ATLAS_SIZE, 1.0f / ATLAS_SIZE);
	case Block::BlockType::Rock:
		return std::pair<float, float>(0.0f, 1.0f / ATLAS_SIZE);
	case Block::BlockType::NumberOfBlockTypes:
	case Block::BlockType::Air:
	default:
		return std::pair<float, float>(0.0f, 0.0f);
	}
}
