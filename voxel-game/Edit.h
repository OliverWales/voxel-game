#pragma once
#include "Block.h"
#include <string>

struct Edit
{
	std::string chunkId;
	int x, y, z;
	Block::BlockType type;
};
