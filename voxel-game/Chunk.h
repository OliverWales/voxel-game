#pragma once
#include "Block.h"

#include <Vector>

constexpr auto CHUNK_SIZE = 16;

class Chunk
{
public:
	Chunk(short int xPos, short int yPos, short int zPos); // initialise chunk seed array
	void generate(Chunk* neighbours[6]); // perform world-gen (requires neighbours' seeds)
	void mesh(); // generate mesh (requires neighbours' blocks)

	long long getId(); // get ID unique to chunk
	float getSeed(int x, int z); // get seed value at (x, z) coordinate
	Block* getBlock(int x, int y, int z); // get block at (x, y, z) coordinate

	std::vector<float> vertices;
	std::vector<unsigned int> indices;

private:
	const short int m_x, m_y, m_z;
	long long m_id;
	void addBlockMesh(int x, int y, int z);
	float m_seed[CHUNK_SIZE][CHUNK_SIZE];
	Block m_blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
	unsigned int m_index = 0;
};
