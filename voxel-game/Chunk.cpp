#include "Chunk.h"

Chunk::Chunk(int xPos, int yPos, int zPos) : xPos(xPos), yPos(yPos), zPos(zPos)
{
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0;	z < CHUNK_SIZE; z++)
			{
				if ((float)rand() / RAND_MAX - 2*(float)y / CHUNK_SIZE > 0.0) {
					m_blocks[x][y][z].type = Block::BlockType::Rock;
				}
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
	vertices.clear();
	indices.clear();

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				if (m_blocks[x][y][z].type == Block::BlockType::Rock) {
					addBlockMesh(x, y, z);
				}
			}
		}
	}
}

void Chunk::addBlockMesh(int x, int y, int z)
{
	float newVertices[] = {
	xPos * CHUNK_SIZE + x + 0.0f, yPos * CHUNK_SIZE + y + 0.0f, zPos * CHUNK_SIZE + z + 1.0f, 0.0f, 0.0f,
	xPos * CHUNK_SIZE + x + 1.0f, yPos * CHUNK_SIZE + y + 0.0f, zPos * CHUNK_SIZE + z + 1.0f, 0.0f, 0.0f,
	xPos * CHUNK_SIZE + x + 1.0f, yPos * CHUNK_SIZE + y + 1.0f, zPos * CHUNK_SIZE + z + 1.0f, 0.0f, 0.0f,
	xPos * CHUNK_SIZE + x + 0.0f, yPos * CHUNK_SIZE + y + 1.0f, zPos * CHUNK_SIZE + z + 1.0f, 0.0f, 0.0f,
	xPos * CHUNK_SIZE + x + 0.0f, yPos * CHUNK_SIZE + y + 0.0f, zPos * CHUNK_SIZE + z + 0.0f, 0.0f, 0.0f,
	xPos * CHUNK_SIZE + x + 1.0f, yPos * CHUNK_SIZE + y + 0.0f, zPos * CHUNK_SIZE + z + 0.0f, 0.0f, 0.0f,
	xPos * CHUNK_SIZE + x + 1.0f, yPos * CHUNK_SIZE + y + 1.0f, zPos * CHUNK_SIZE + z + 0.0f, 0.0f, 0.0f,
	xPos * CHUNK_SIZE + x + 0.0f, yPos * CHUNK_SIZE + y + 1.0f, zPos * CHUNK_SIZE + z + 0.0f, 0.0f, 0.0f
	};
	vertices.insert(std::end(vertices), std::begin(newVertices), std::end(newVertices));

	// 6 cases
	if (x == 0 || m_blocks[x - 1][y][z].type == Block::BlockType::Air) {
		unsigned int newIndices[] = {
			m_index + 4, m_index + 0, m_index + 3,
			m_index + 3, m_index + 7, m_index + 4
		};
		
		indices.insert(std::end(indices), std::begin(newIndices), std::end(newIndices));
	}

	if (y == 0 || m_blocks[x][y - 1][z].type == Block::BlockType::Air) {
		unsigned int newIndices[] = {
			m_index + 4, m_index + 5, m_index + 1,
			m_index + 1, m_index + 0, m_index + 4
		};

		indices.insert(std::end(indices), std::begin(newIndices), std::end(newIndices));
	}

	if (z == 0 || m_blocks[x][y][z - 1].type == Block::BlockType::Air) {
		unsigned int newIndices[] = {
			m_index + 7, m_index + 6, m_index + 5,
			m_index + 5, m_index + 4, m_index + 7
		};

		indices.insert(std::end(indices), std::begin(newIndices), std::end(newIndices));
	}

	if (x == CHUNK_SIZE - 1 || m_blocks[x + 1][y][z].type == Block::BlockType::Air) {
		unsigned int newIndices[] = {
			m_index + 1, m_index + 5, m_index + 6,
			m_index + 6, m_index + 2, m_index + 1
		};

		indices.insert(std::end(indices), std::begin(newIndices), std::end(newIndices));
	}

	if (y == CHUNK_SIZE - 1 || m_blocks[x][y + 1][z].type == Block::BlockType::Air) {
		unsigned int newIndices[] = {
			m_index + 3, m_index + 2, m_index + 6,
			m_index + 6, m_index + 7, m_index + 3
		};

		indices.insert(std::end(indices), std::begin(newIndices), std::end(newIndices));
	}

	if (z == CHUNK_SIZE - 1 || m_blocks[x][y][z + 1].type == Block::BlockType::Air) {
		unsigned int newIndices[] = {
			m_index + 0, m_index + 1, m_index + 2,
			m_index + 2, m_index + 3, m_index + 0
		};

		indices.insert(std::end(indices), std::begin(newIndices), std::end(newIndices));
	}

	m_index += 8;
}
