#pragma once

class Block
{
public:
	enum class BlockType {
		Air,
		Rock
	};

	Block(BlockType type = BlockType::Air);
	BlockType type;
};
