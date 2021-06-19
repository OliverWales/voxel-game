#include "Chunk.h"

Chunk::Chunk(int x, int y, int z)
	: x(x)
	, y(y)
	, z(z)
	, id(std::to_string(x) + std::to_string(y) + std::to_string(z))
{
	srand(std::hash<std::string>{}(id));

	for (int xIndex = 0; xIndex < CHUNK_SIZE; xIndex++)
	{
		for (int zIndex = 0; zIndex < CHUNK_SIZE; zIndex++)
		{
			m_seed[xIndex][zIndex] = (float)rand() / RAND_MAX;
		}
	}
}

Chunk::~Chunk()
{
	indices.clear();
	vertices.clear();
	id.empty();
}

void Chunk::generate(Chunk* xNeighbour, Chunk* zNeighbour, Chunk* xzNeighbour, std::vector<Edit>* edits)
{
	if (m_generated) return;
	m_generated = true;

	float smooth_noise[CHUNK_SIZE][CHUNK_SIZE];
	int octaves = 4;
	float bias = 3.5f;

	for (int xIndex = 0; xIndex < CHUNK_SIZE; xIndex++)
	{
		for (int zIndex = 0; zIndex < CHUNK_SIZE; zIndex++)
		{
			float noise = 0.0f;
			float scale = 1.0f;
			float scaleTot = 0.0f;

			for (int octave = 0; octave < octaves; octave++)
			{
				int stride = CHUNK_SIZE >> octave;
				int x1 = (xIndex / stride) * stride;
				int z1 = (zIndex / stride) * stride;

				int x2 = x1 + stride;
				int z2 = z1 + stride;

				float blendX = (float)(xIndex - x1) / (float)stride;
				float blendZ = (float)(zIndex - z1) / (float)stride;

				float x1z1 = m_seed[x1][z1];
				float x1z2 = z2 < CHUNK_SIZE ? m_seed[x1][z2] : zNeighbour->getSeed(x1, z2 - CHUNK_SIZE);
				float x2z1 = x2 < CHUNK_SIZE ? m_seed[x2][z1] : xNeighbour->getSeed(x2 - CHUNK_SIZE, z1);
				float x2z2;
				if (x2 < CHUNK_SIZE && z2 < CHUNK_SIZE) {
					x2z2 = m_seed[x2][z2];
				}
				else if (x2 < CHUNK_SIZE) {
					x2z2 = zNeighbour->getSeed(x2, z2 - CHUNK_SIZE);
				}
				else if (z2 < CHUNK_SIZE) {
					x2z2 = xNeighbour->getSeed(x2 - CHUNK_SIZE, z2);
				}
				else {
					x2z2 = xzNeighbour->getSeed(x2 - CHUNK_SIZE, z2 - CHUNK_SIZE);
				}

				float fSampleT = (1.0f - blendX) * x1z1 + blendX * x2z1;
				float fSampleB = (1.0f - blendX) * x1z2 + blendX * x2z2;

				noise += (blendZ * (fSampleB - fSampleT) + fSampleT) * scale;
				scaleTot += scale;
				scale /= bias;
			}

			// Normalise to range 0-1
			smooth_noise[xIndex][zIndex] = noise / scaleTot;
		}
	}

	bool empty = true;
	for (int xIndex = 0; xIndex < CHUNK_SIZE; xIndex++)
	{
		for (int yIndex = 0; yIndex < CHUNK_SIZE; yIndex++)
		{
			for (int zIndex = 0; zIndex < CHUNK_SIZE; zIndex++)
			{
				int h = CHUNK_SIZE * y + smooth_noise[xIndex][zIndex] * 15;
				if (yIndex + h > 15) {
					m_blocks[xIndex][yIndex][zIndex].type = Block::BlockType::Air;
				}
				else if (yIndex + h == 15) {
					m_blocks[xIndex][yIndex][zIndex].type = Block::BlockType::Grass;
					m_empty = false;
				}
				else if (yIndex + h > 12) {
					m_blocks[xIndex][yIndex][zIndex].type = Block::BlockType::Dirt;
					m_empty = false;
				}
				else {
					m_blocks[xIndex][yIndex][zIndex].type = Block::BlockType::Stone;
					m_empty = false;
				}
			}
		}
	}

	for (auto edit : *edits) {
		if (edit.chunkId == id) {
			m_blocks[edit.x][edit.y][edit.z] = edit.type;
		}
	}
}

void Chunk::mesh()
{
	if (m_empty) {
		m_meshed = true;
		vertices.clear();
		indices.clear();
		return;
	}

	if (m_meshed) return;
	remesh();
}

void Chunk::remesh()
{
	// TODO: Pass blocks of neighbouring chunks and avoid generating uneccesary faces at chunk boundaries
	m_meshed = true;

	vertices.clear();
	indices.clear();
	m_index = 0;

	for (int xIndex = 0; xIndex < CHUNK_SIZE; xIndex++)
	{
		for (int yIndex = 0; yIndex < CHUNK_SIZE; yIndex++)
		{
			for (int zIndex = 0; zIndex < CHUNK_SIZE; zIndex++)
			{
				if (m_blocks[xIndex][yIndex][zIndex].type != Block::BlockType::Air) {
					addBlockMesh(xIndex, yIndex, zIndex);
				}
			}
		}
	}
}

bool Chunk::isMeshed()
{
	return m_meshed;
}

float Chunk::getSeed(int xIndex, int zIndex)
{
	return m_seed[xIndex][zIndex];
}

Block* Chunk::getBlock(int xIndex, int yIndex, int zIndex)
{
	return &m_blocks[xIndex][yIndex][zIndex];
}

RayCastHit Chunk::getBlock(glm::vec3 origin, glm::vec3 direction, float maxDist)
{
	origin -= glm::vec3(x * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE);

	int x = floor(origin.x);
	int y = floor(origin.y);
	int z = floor(origin.z);

	float dx = direction.x;
	float dy = direction.y;
	float dz = direction.z;

	if (dx == 0 && dy == 0 && dz == 0)
		return { 0, 0, 0, -1 };

	int stepX = dx >= 0 ? 1 : -1;
	int stepY = dy >= 0 ? 1 : -1;
	int stepZ = dz >= 0 ? 1 : -1;

	float maxX = bound(origin.x, dx);
	float maxY = bound(origin.y, dy);
	float maxZ = bound(origin.z, dz);

	float deltaX = stepX / dx;
	float deltaY = stepY / dy;
	float deltaZ = stepZ / dz;

	int normalX = 0;
	int normalY = 0;
	int normalZ = 0;

	while (stepX > 0 ? x < CHUNK_SIZE : x >= 0
		&& stepY > 0 ? y < CHUNK_SIZE : y >= 0
		&& stepZ > 0 ? z < CHUNK_SIZE : z >= 0) {

		if (!(x < 0 || y < 0 || z < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE))
			if (m_blocks[x][y][z].type != Block::BlockType::Air)
				return { true, x, y, z, normalX, normalY, normalZ };

		if (maxX < maxY) {
			if (maxX < maxZ) {
				normalX = -stepX;
				normalY = 0;
				normalZ = 0;
				if (maxX > maxDist) return { false, 0, 0, 0, 0, 0, 0 };
				x += stepX;
				maxX += deltaX;
			}
			else {
				normalX = 0;
				normalY = 0;
				normalZ = -stepZ;
				if (maxZ > maxDist) return { false, 0, 0, 0, 0, 0, 0 };
				z += stepZ;
				maxZ += deltaZ;
			}
		}
		else {
			if (maxY < maxZ) {
				normalX = 0;
				normalY = -stepY;
				normalZ = 0;
				if (maxY > maxDist) return { false, 0, 0, 0, 0, 0, 0 };
				y += stepY;
				maxY += deltaY;
			}
			else {
				normalX = 0;
				normalY = 0;
				normalZ = -stepZ;
				if (maxZ > maxDist) return { false, 0, 0, 0, 0, 0, 0 };
				z += stepZ;
				maxZ += deltaZ;
			}
		}
	}

	return { false, 0, 0, 0, normalX, normalY, normalZ };
}

void Chunk::setBlock(int x, int y, int z, Block::BlockType type)
{
	m_blocks[x][y][z].type = type;
}

std::string Chunk::getId(int x, int y, int z)
{
	return std::to_string(x) + std::to_string(y) + std::to_string(z);
}

void Chunk::addBlockMesh(int xIndex, int yIndex, int zIndex)
{
	Block block = m_blocks[xIndex][yIndex][zIndex];

	// side 0: x = 0 (RIGHT)
	if (xIndex == 0 || m_blocks[xIndex - 1][yIndex][zIndex].type == Block::BlockType::Air) {
		std::pair<float, float> uv = block.getUV(0);
		float u = uv.first;
		float v = uv.second;
		float newVertices[] = {
			x * CHUNK_SIZE + xIndex + 0.0f, y * CHUNK_SIZE + yIndex + 0.0f, z * CHUNK_SIZE + zIndex + 0.0f, u + 0.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE, -1.0f, 0.0f, 0.0f,
			x * CHUNK_SIZE + xIndex + 0.0f, y * CHUNK_SIZE + yIndex + 1.0f, z * CHUNK_SIZE + zIndex + 0.0f, u + 0.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE, -1.0f, 0.0f, 0.0f,
			x * CHUNK_SIZE + xIndex + 0.0f, y * CHUNK_SIZE + yIndex + 1.0f, z * CHUNK_SIZE + zIndex + 1.0f, u + 1.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE, -1.0f, 0.0f, 0.0f,
			x * CHUNK_SIZE + xIndex + 0.0f, y * CHUNK_SIZE + yIndex + 0.0f, z * CHUNK_SIZE + zIndex + 1.0f, u + 1.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE, -1.0f, 0.0f, 0.0f
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
	if (yIndex == 0 || m_blocks[xIndex][yIndex - 1][zIndex].type == Block::BlockType::Air) {
		std::pair<float, float> uv = block.getUV(1);
		float u = uv.first;
		float v = uv.second;
		float newVertices[] = {
			x * CHUNK_SIZE + xIndex + 1.0f, y * CHUNK_SIZE + yIndex + 0.0f, z * CHUNK_SIZE + zIndex + 1.0f, u + 0.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE, 0.0f, -1.0f, 0.0f,
			x * CHUNK_SIZE + xIndex + 1.0f, y * CHUNK_SIZE + yIndex + 0.0f, z * CHUNK_SIZE + zIndex + 0.0f, u + 0.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE, 0.0f, -1.0f, 0.0f,
			x * CHUNK_SIZE + xIndex + 0.0f, y * CHUNK_SIZE + yIndex + 0.0f, z * CHUNK_SIZE + zIndex + 0.0f, u + 1.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE, 0.0f, -1.0f, 0.0f,
			x * CHUNK_SIZE + xIndex + 0.0f, y * CHUNK_SIZE + yIndex + 0.0f, z * CHUNK_SIZE + zIndex + 1.0f, u + 1.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE, 0.0f, -1.0f, 0.0f
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
	if (zIndex == 0 || m_blocks[xIndex][yIndex][zIndex - 1].type == Block::BlockType::Air) {
		std::pair<float, float> uv = block.getUV(2);
		float u = uv.first;
		float v = uv.second;
		float newVertices[] = {
			x * CHUNK_SIZE + xIndex + 1.0f, y * CHUNK_SIZE + yIndex + 0.0f, z * CHUNK_SIZE + zIndex + 0.0f, u + 0.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE, 0.0f, 0.0f, -1.0f,
			x * CHUNK_SIZE + xIndex + 1.0f, y * CHUNK_SIZE + yIndex + 1.0f, z * CHUNK_SIZE + zIndex + 0.0f, u + 0.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE, 0.0f, 0.0f, -1.0f,
			x * CHUNK_SIZE + xIndex + 0.0f, y * CHUNK_SIZE + yIndex + 1.0f, z * CHUNK_SIZE + zIndex + 0.0f, u + 1.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE, 0.0f, 0.0f, -1.0f,
			x * CHUNK_SIZE + xIndex + 0.0f, y * CHUNK_SIZE + yIndex + 0.0f, z * CHUNK_SIZE + zIndex + 0.0f, u + 1.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE, 0.0f, 0.0f, -1.0f
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
	if (xIndex == CHUNK_SIZE - 1 || m_blocks[xIndex + 1][yIndex][zIndex].type == Block::BlockType::Air) {
		std::pair<float, float> uv = block.getUV(3);
		float u = uv.first;
		float v = uv.second;
		float newVertices[] = {
			x * CHUNK_SIZE + xIndex + 1.0f, y * CHUNK_SIZE + yIndex + 0.0f, z * CHUNK_SIZE + zIndex + 1.0f, u + 0.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE, 1.0f, 0.0f, 0.0f,
			x * CHUNK_SIZE + xIndex + 1.0f, y * CHUNK_SIZE + yIndex + 1.0f, z * CHUNK_SIZE + zIndex + 1.0f, u + 0.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE, 1.0f, 0.0f, 0.0f,
			x * CHUNK_SIZE + xIndex + 1.0f, y * CHUNK_SIZE + yIndex + 1.0f, z * CHUNK_SIZE + zIndex + 0.0f, u + 1.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE, 1.0f, 0.0f, 0.0f,
			x * CHUNK_SIZE + xIndex + 1.0f, y * CHUNK_SIZE + yIndex + 0.0f, z * CHUNK_SIZE + zIndex + 0.0f, u + 1.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE, 1.0f, 0.0f, 0.0f
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
	if (yIndex == CHUNK_SIZE - 1 || m_blocks[xIndex][yIndex + 1][zIndex].type == Block::BlockType::Air) {
		std::pair<float, float> uv = block.getUV(4);
		float u = uv.first;
		float v = uv.second;
		float newVertices[] = {
			x * CHUNK_SIZE + xIndex + 1.0f, y * CHUNK_SIZE + yIndex + 1.0f, z * CHUNK_SIZE + zIndex + 0.0f, u + 0.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE, 0.0f, 1.0f, 0.0f,
			x * CHUNK_SIZE + xIndex + 1.0f, y * CHUNK_SIZE + yIndex + 1.0f, z * CHUNK_SIZE + zIndex + 1.0f, u + 0.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE, 0.0f, 1.0f, 0.0f,
			x * CHUNK_SIZE + xIndex + 0.0f, y * CHUNK_SIZE + yIndex + 1.0f, z * CHUNK_SIZE + zIndex + 1.0f, u + 1.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE, 0.0f, 1.0f, 0.0f,
			x * CHUNK_SIZE + xIndex + 0.0f, y * CHUNK_SIZE + yIndex + 1.0f, z * CHUNK_SIZE + zIndex + 0.0f, u + 1.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE, 0.0f, 1.0f, 0.0f
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
	if (zIndex == CHUNK_SIZE - 1 || m_blocks[xIndex][yIndex][zIndex + 1].type == Block::BlockType::Air) {
		std::pair<float, float> uv = block.getUV(5);
		float u = uv.first;
		float v = uv.second;
		float newVertices[] = {
			x * CHUNK_SIZE + xIndex + 0.0f, y * CHUNK_SIZE + yIndex + 0.0f, z * CHUNK_SIZE + zIndex + 1.0f, u + 0.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE, 0.0f, 0.0f, 1.0f,
			x * CHUNK_SIZE + xIndex + 0.0f, y * CHUNK_SIZE + yIndex + 1.0f, z * CHUNK_SIZE + zIndex + 1.0f, u + 0.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE, 0.0f, 0.0f, 1.0f,
			x * CHUNK_SIZE + xIndex + 1.0f, y * CHUNK_SIZE + yIndex + 1.0f, z * CHUNK_SIZE + zIndex + 1.0f, u + 1.0f / ATLAS_SIZE, v + 0.0f / ATLAS_SIZE, 0.0f, 0.0f, 1.0f,
			x * CHUNK_SIZE + xIndex + 1.0f, y * CHUNK_SIZE + yIndex + 0.0f, z * CHUNK_SIZE + zIndex + 1.0f, u + 1.0f / ATLAS_SIZE, v + 1.0f / ATLAS_SIZE, 0.0f, 0.0f, 1.0f
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

int Chunk::bound(float s, float ds)
{
	if (ds < 0) {
		return bound(-s, -ds);
	}
	else {
		s = s - int(s);
		return (1 - s) / ds;
	}
}
