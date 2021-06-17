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
		x + 0.0f, y + 0.0f, z + 1.0f, 0.0f, 0.0f,
		x + 1.0f, y + 0.0f, z + 1.0f, 0.0f, 0.0f,
		x + 1.0f, y + 1.0f, z + 1.0f, 0.0f, 0.0f,
		x + 0.0f, y + 1.0f, z + 1.0f, 0.0f, 0.0f,
		x + 0.0f, y + 0.0f, z + 0.0f, 0.0f, 0.0f,
		x + 1.0f, y + 0.0f, z + 0.0f, 0.0f, 0.0f,
		x + 1.0f, y + 1.0f, z + 0.0f, 0.0f, 0.0f,
		x + 0.0f, y + 1.0f, z + 0.0f, 0.0f, 0.0f
	};
	vertices.insert(std::end(vertices), std::begin(newVertices), std::end(newVertices));

	unsigned int newIndices[] = {
		m_index + 0, m_index + 1, m_index + 2,
		m_index + 2, m_index + 3, m_index + 0,
		m_index + 1, m_index + 5, m_index + 6,
		m_index + 6, m_index + 2, m_index + 1,
		m_index + 7, m_index + 6, m_index + 5,
		m_index + 5, m_index + 4, m_index + 7,
		m_index + 4, m_index + 0, m_index + 3,
		m_index + 3, m_index + 7, m_index + 4,
		m_index + 4, m_index + 5, m_index + 1,
		m_index + 1, m_index + 0, m_index + 4,
		m_index + 3, m_index + 2, m_index + 6,
		m_index + 6, m_index + 7, m_index + 3
	};
	indices.insert(std::end(indices), std::begin(newIndices), std::end(newIndices));
	m_index += 8;
}
