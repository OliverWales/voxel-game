#pragma once
#include "Block.h"
#include "Edit.h"

#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <Vector>

constexpr auto CHUNK_SIZE = 16;

class Chunk
{
public:
	Chunk(int x, int y, int z); // initialise chunk seed array
	~Chunk();
	void generate(Chunk* xNeighbour, Chunk* zNeighbour, Chunk* xzNeighbour, std::vector<Edit>* edits); // perform world-gen (requires neighbours' seeds to make noise continuous)
	void mesh(); // generate mesh
	void remesh(); // regenerate mesh

	struct RayCastHit {
		int xIndex, yIndex, zIndex;
		int face;
	};

	bool isMeshed();
	float getSeed(int xIndex, int zIndex); // get seed value at (x, z) coordinate
	Block* getBlock(int xIndex, int yIndex, int zIndex); // get block at (x, y, z) coordinate
	RayCastHit getBlock(glm::vec3 origin, glm::vec3 direction, float maxDist);
	void setBlock(int x, int y, int z, Block::BlockType type);

	static std::string getId(int x, int y, int z);

	const int x, y, z;
	const std::string id;
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

private:
	void addBlockMesh(int xIndex, int yIndex, int zIndex);
	int bound(float s, float ds);

	float m_seed[CHUNK_SIZE][CHUNK_SIZE];
	Block m_blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
	bool m_generated = false;
	bool m_meshed = false;
	unsigned int m_index = 0;
};
