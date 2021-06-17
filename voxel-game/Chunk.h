#pragma once
#include "Block.h"

#include <Vector>

#define CHUNK_SIZE 8

class Chunk
{
public:
	Chunk();
	~Chunk() = default;
	Block get(int x, int y, int z);
	void remesh();
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

private:
	void addBlockMesh(int x, int y, int z);

	Block m_blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
	unsigned int m_index = 0;
};
