#pragma once
#include "Block.h"

#include <Vector>

#define CHUNK_SIZE 8

class Chunk
{
public:
	Chunk(int xPos, int yPos, int zPos);
	Block get(int x, int y, int z);
	void remesh();
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	const int xPos, yPos, zPos;

private:
	void addBlockMesh(int x, int y, int z);

	Block m_blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
	unsigned int m_index = 0;
};
