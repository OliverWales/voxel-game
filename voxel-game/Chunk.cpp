#include "Chunk.h"

Chunk::Chunk()
{
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0;	z < CHUNK_SIZE; z++)
			{
				m_blocks[x][y][z].type = Block::BlockType::Rock;
			}
		}
	}

	remesh();
}

Block Chunk::get(int x, int y, int z)
{
	return m_blocks[x][y][z];
}

void Chunk::remesh()
{
	// TODO: Actually mesh the chunk
	vertices = {
		// front
		-1.0, -1.0,  1.0, 0.0, 0.0,
		 1.0, -1.0,  1.0, 0.0, 0.0,
		 1.0,  1.0,  1.0, 0.0, 0.0,
		-1.0,  1.0,  1.0, 0.0, 0.0,
		// back
		-1.0, -1.0, -1.0, 0.0, 0.0,
		 1.0, -1.0, -1.0, 0.0, 0.0,
		 1.0,  1.0, -1.0, 0.0, 0.0,
		-1.0,  1.0, -1.0, 0.0, 0.0
	};

	indices = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};
}
