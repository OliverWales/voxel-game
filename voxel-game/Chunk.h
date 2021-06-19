#pragma once
#include "Block.h"

#include <iostream>
#include <string>
#include <Vector>

constexpr auto CHUNK_SIZE = 16;

class Chunk
{
public:
	Chunk(int x, int y, int z); // initialise chunk seed array
	~Chunk();
	void generate(Chunk* xNeighbour, Chunk* zNeighbour, Chunk* xzNeighbour); // perform world-gen (requires neighbours' seeds to make noise continuous)
	void mesh(); // generate mesh

	bool isMeshed();
	float getSeed(int xIndex, int zIndex); // get seed value at (x, z) coordinate
	Block* getBlock(int xIndex, int yIndex, int zIndex); // get block at (x, y, z) coordinate

	static std::string getId(int x, int y, int z);

	const int x, y, z;
	const std::string id;
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

private:
	void addBlockMesh(int xIndex, int yIndex, int zIndex);
	float m_seed[CHUNK_SIZE][CHUNK_SIZE];
	Block m_blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
	bool m_generated = false;
	bool m_meshed = false;
	unsigned int m_index = 0;
};
