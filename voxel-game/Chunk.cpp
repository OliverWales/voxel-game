#include "Chunk.h"

Chunk::Chunk(int xPos, int yPos, int zPos) : xPos(xPos), yPos(yPos), zPos(zPos)
{
	int height[CHUNK_SIZE][CHUNK_SIZE];

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			height[x][z] = (int)8 + 4 * (float)rand() / RAND_MAX;
		}
	}

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0;	z < CHUNK_SIZE; z++)
			{
				if (y == height[x][z]) {
					m_blocks[x][y][z].type = Block::BlockType::Grass;
				}
				else if (y < height[x][z] - 4) {
					m_blocks[x][y][z].type = Block::BlockType::Stone;
				}
				else if (y < height[x][z]) {
					m_blocks[x][y][z].type = Block::BlockType::Dirt;
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
				if (m_blocks[x][y][z].type != Block::BlockType::Air) {
					addBlockMesh(x, y, z);
				}
			}
		}
	}
}

void Chunk::addBlockMesh(int x, int y, int z)
{
	Block block = m_blocks[x][y][z];

	// side 0: x = 0 (RIGHT)
	if (x == 0 || m_blocks[x - 1][y][z].type == Block::BlockType::Air) {
		std::pair<float, float> uv = block.getUV(0);
		float u = uv.first;
		float v = uv.second;
		float newVertices[] = {
			xPos * CHUNK_SIZE + x + 0.0f, yPos * CHUNK_SIZE + y + 0.0f, zPos * CHUNK_SIZE + z + 0.0f, u + 0.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE,
			xPos * CHUNK_SIZE + x + 0.0f, yPos * CHUNK_SIZE + y + 1.0f, zPos * CHUNK_SIZE + z + 0.0f, u + 0.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE,
			xPos * CHUNK_SIZE + x + 0.0f, yPos * CHUNK_SIZE + y + 1.0f, zPos * CHUNK_SIZE + z + 1.0f, u + 1.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE,
			xPos * CHUNK_SIZE + x + 0.0f, yPos * CHUNK_SIZE + y + 0.0f, zPos * CHUNK_SIZE + z + 1.0f, u + 1.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE,
		};

		vertices.insert(std::end(vertices), std::begin(newVertices), std::end(newVertices));
		 
		unsigned int newIndices[] = {
			m_index + 0, m_index + 2, m_index + 1,
			m_index + 0, m_index + 3, m_index + 2
		};
		
		indices.insert(std::end(indices), std::begin(newIndices), std::end(newIndices));
		m_index += 4;
	}

	// side 1: y = 0 (BOTTOM)
	if (y == 0 || m_blocks[x][y - 1][z].type == Block::BlockType::Air) {
		std::pair<float, float> uv = block.getUV(1);
		float u = uv.first;
		float v = uv.second;
		float newVertices[] = {
			xPos * CHUNK_SIZE + x + 1.0f, yPos * CHUNK_SIZE + y + 0.0f, zPos * CHUNK_SIZE + z + 1.0f, u + 0.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE,
			xPos * CHUNK_SIZE + x + 1.0f, yPos * CHUNK_SIZE + y + 0.0f, zPos * CHUNK_SIZE + z + 0.0f, u + 0.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE,
			xPos * CHUNK_SIZE + x + 0.0f, yPos * CHUNK_SIZE + y + 0.0f, zPos * CHUNK_SIZE + z + 0.0f, u + 1.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE,
			xPos * CHUNK_SIZE + x + 0.0f, yPos * CHUNK_SIZE + y + 0.0f, zPos * CHUNK_SIZE + z + 1.0f, u + 1.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE,
		};

		vertices.insert(std::end(vertices), std::begin(newVertices), std::end(newVertices));

		unsigned int newIndices[] = {
			m_index + 0, m_index + 2, m_index + 1,
			m_index + 0, m_index + 3, m_index + 2
		};

		indices.insert(std::end(indices), std::begin(newIndices), std::end(newIndices));
		m_index += 4;
	}

	// side 2: z = 0 (BACK)
	if (z == 0 || m_blocks[x][y][z - 1].type == Block::BlockType::Air) {
		std::pair<float, float> uv = block.getUV(2);
		float u = uv.first;
		float v = uv.second;
		float newVertices[] = {
			xPos * CHUNK_SIZE + x + 1.0f, yPos * CHUNK_SIZE + y + 0.0f, zPos * CHUNK_SIZE + z + 0.0f, u + 0.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE,
			xPos * CHUNK_SIZE + x + 1.0f, yPos * CHUNK_SIZE + y + 1.0f, zPos * CHUNK_SIZE + z + 0.0f, u + 0.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE,
			xPos * CHUNK_SIZE + x + 0.0f, yPos * CHUNK_SIZE + y + 1.0f, zPos * CHUNK_SIZE + z + 0.0f, u + 1.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE,
			xPos * CHUNK_SIZE + x + 0.0f, yPos * CHUNK_SIZE + y + 0.0f, zPos * CHUNK_SIZE + z + 0.0f, u + 1.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE,
		};

		vertices.insert(std::end(vertices), std::begin(newVertices), std::end(newVertices));

		unsigned int newIndices[] = {
			m_index + 0, m_index + 2, m_index + 1,
			m_index + 0, m_index + 3, m_index + 2
		};

		indices.insert(std::end(indices), std::begin(newIndices), std::end(newIndices));
		m_index += 4;
	}

	// side 3: x = 1 (RIGHT)
	if (x == CHUNK_SIZE - 1 || m_blocks[x + 1][y][z].type == Block::BlockType::Air) {
		std::pair<float, float> uv = block.getUV(3);
		float u = uv.first;
		float v = uv.second;
		float newVertices[] = {
			xPos * CHUNK_SIZE + x + 1.0f, yPos * CHUNK_SIZE + y + 0.0f, zPos * CHUNK_SIZE + z + 1.0f, u + 0.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE,
			xPos * CHUNK_SIZE + x + 1.0f, yPos * CHUNK_SIZE + y + 1.0f, zPos * CHUNK_SIZE + z + 1.0f, u + 0.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE,
			xPos * CHUNK_SIZE + x + 1.0f, yPos * CHUNK_SIZE + y + 1.0f, zPos * CHUNK_SIZE + z + 0.0f, u + 1.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE,
			xPos * CHUNK_SIZE + x + 1.0f, yPos * CHUNK_SIZE + y + 0.0f, zPos * CHUNK_SIZE + z + 0.0f, u + 1.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE,
		};

		vertices.insert(std::end(vertices), std::begin(newVertices), std::end(newVertices));

		unsigned int newIndices[] = {
			m_index + 0, m_index + 2, m_index + 1,
			m_index + 0, m_index + 3, m_index + 2
		};

		indices.insert(std::end(indices), std::begin(newIndices), std::end(newIndices));
		m_index += 4;
	}

	// side 4: y = 1 (TOP)
	if (y == CHUNK_SIZE - 1 || m_blocks[x][y + 1][z].type == Block::BlockType::Air) {
		std::pair<float, float> uv = block.getUV(4);
		float u = uv.first;
		float v = uv.second;
		float newVertices[] = {
			xPos * CHUNK_SIZE + x + 1.0f, yPos * CHUNK_SIZE + y + 1.0f, zPos * CHUNK_SIZE + z + 0.0f, u + 0.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE,
			xPos * CHUNK_SIZE + x + 1.0f, yPos * CHUNK_SIZE + y + 1.0f, zPos * CHUNK_SIZE + z + 1.0f, u + 0.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE,
			xPos * CHUNK_SIZE + x + 0.0f, yPos * CHUNK_SIZE + y + 1.0f, zPos * CHUNK_SIZE + z + 1.0f, u + 1.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE,
			xPos * CHUNK_SIZE + x + 0.0f, yPos * CHUNK_SIZE + y + 1.0f, zPos * CHUNK_SIZE + z + 0.0f, u + 1.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE,
		};

		vertices.insert(std::end(vertices), std::begin(newVertices), std::end(newVertices));

		unsigned int newIndices[] = {
			m_index + 0, m_index + 2, m_index + 1,
			m_index + 0, m_index + 3, m_index + 2
		};

		indices.insert(std::end(indices), std::begin(newIndices), std::end(newIndices));
		m_index += 4;
	}

	// side 5: z = 1 (FRONT)
	if (z == CHUNK_SIZE - 1 || m_blocks[x][y][z + 1].type == Block::BlockType::Air) {
		std::pair<float, float> uv = block.getUV(5);
		float u = uv.first;
		float v = uv.second;
		float newVertices[] = {
			xPos * CHUNK_SIZE + x + 0.0f, yPos * CHUNK_SIZE + y + 0.0f, zPos * CHUNK_SIZE + z + 1.0f, u + 0.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE,
			xPos * CHUNK_SIZE + x + 0.0f, yPos * CHUNK_SIZE + y + 1.0f, zPos * CHUNK_SIZE + z + 1.0f, u + 0.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE,
			xPos * CHUNK_SIZE + x + 1.0f, yPos * CHUNK_SIZE + y + 1.0f, zPos * CHUNK_SIZE + z + 1.0f, u + 1.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE,
			xPos * CHUNK_SIZE + x + 1.0f, yPos * CHUNK_SIZE + y + 0.0f, zPos * CHUNK_SIZE + z + 1.0f, u + 1.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE,
		};

		vertices.insert(std::end(vertices), std::begin(newVertices), std::end(newVertices));

		unsigned int newIndices[] = {
			m_index + 0, m_index + 2, m_index + 1,
			m_index + 0, m_index + 3, m_index + 2
		};

		indices.insert(std::end(indices), std::begin(newIndices), std::end(newIndices));
		m_index += 4;
	}
}
