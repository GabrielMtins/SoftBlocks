#include "World.hpp"
#include "Draw.hpp"

#include <cstdlib>
#include <unordered_set>

std::unordered_set<uint8_t> not_solid_blocks = {
	BLOCK_AIR
};

World::World(void) {
	for(auto& i : blocks) {
		i = 0;
	}
}

uint8_t World::getBlock(int x, int y, int z) const {
	if(!checkBoundaries(x, y, z))
		return 0;

	return blocks[getIndex(x, y, z)];
}

bool World::hasSolidBlock(int x, int y, int z) const {
	uint8_t block = getBlock(x, y, z);

	return not_solid_blocks.find(block) == not_solid_blocks.end();
}

void World::setBlock(int x, int y, int z, int block) {
	if(!checkBoundaries(x, y, z))
		return;

	blocks[getIndex(x, y, z)] = block;
}

bool World::checkBoundaries(int x, int y, int z) const {
	if(x < 0)
		return false;

	if(y < 0)
		return false;

	if(z < 0)
		return false;

	if(x >= WORLD_SIZE)
		return false;

	if(y >= WORLD_HEIGHT)
		return false;

	if(z >= WORLD_SIZE)
		return false;

	return true;
}

void World::generate(void) {
	Draw tmp;
	height_map.createScreen(WORLD_SIZE, WORLD_SIZE);
	tmp.createScreen(WORLD_SIZE, WORLD_SIZE);

	for(int i = 0; i < WORLD_SIZE; i++) {
		for(int j = 0; j < WORLD_SIZE; j++) {
			height_map.putPixel(i, j, rand() % WORLD_HEIGHT);
		}
	}

	for(int k = 0; k < WORLD_SMOOTHNESS; k++) {
		for(int i = 0; i < WORLD_SIZE; i++) {
			for(int j = 0; j < WORLD_SIZE; j++) {
				tmp.putPixel(
						i,
						j,
						getAverage(height_map, i, j, WORLD_RADIUS_SMOOTH)
						);
			}
		}

		for(int i = 0; i < WORLD_SIZE; i++) {
			for(int j = 0; j < WORLD_SIZE; j++) {
				height_map.putPixel(
						i,
						j,
						tmp.getPixel(i, j)
						);
			}
		}
	}

	for(int i = 0; i < WORLD_SIZE; i++) {
		for(int j = 0; j < WORLD_SIZE; j++) {
			int height = height_map.getPixel(i, j);

			for(int k = 0; k < std::min(height, WORLD_SIZE); k++) {
				setBlock(i, k, j, getBlockType(k, height));
			}
		}
	}

	for(int i = 0; i < NUM_TREE; i++) {
		generateTree(height_map);
	}
}

int World::getHeight(int x, int z) const {
	return height_map.getPixel(x, z);
}

int World::getIndex(int x, int y, int z) const {
	return x + y * WORLD_SIZE * WORLD_SIZE + z * WORLD_SIZE;
}

int World::getBlockType(int y, int height) const {
	if(y < 120)
		return BLOCK_STONE;

	if(y < 123) {
		if(y == height - 1) {
			return BLOCK_SAND;
		}
	}

	if(y < height - 2) {
		return BLOCK_DIRT;
	}

	return BLOCK_GRASS;
}

void World::generateTree(const Draw& draw) {
	int x, y, z;

	x = rand() % WORLD_SIZE;
	z = rand() % WORLD_SIZE;

	y = draw.getPixel(x, z) - 1;

	if(getBlock(x, y, z) != BLOCK_GRASS) {
		return;
	}

	for(int i = 1; i < 6; i++) { 
		setBlock(x, y + i, z, BLOCK_WOOD);
	}

	for(int i = -2; i <= 2; i++) {
		for(int j = -2; j <= 2; j++) {
			if(i == 0 && j == 0)
				continue;

			setBlock(
					x + i,
					y + 4,
					z + j,
					BLOCK_LEAF
					);

			setBlock(
					x + i,
					y + 5,
					z + j,
					BLOCK_LEAF
					);
		}
	}

	for(int i = -1; i <= 1; i++) {
		for(int j = -1; j <= 1; j++) {
			setBlock(
					x + i,
					y + 6,
					z + j,
					BLOCK_LEAF
					);

			if(std::abs(i) != std::abs(j)) {
				setBlock(
						x + i,
						y + 7,
						z + j,
						BLOCK_LEAF
						);
			}

			if(i == 0 && j == 0) {
				setBlock(
						x + i,
						y + 7,
						z + j,
						BLOCK_LEAF
						);
			}
		}
	}
}

uint32_t World::getAverage(const Draw& draw, int x, int y, int radius) {
	int sum = 0;

	for(int j = -radius + y; j < radius + y; j++) {
		for(int i = -radius + x; i < radius + x; i++) {
			if(!draw.checkBoundaries(i, j)) {
				sum += WORLD_HEIGHT / 2;
			} else {
				sum += draw.getPixel(i, j);
			}
		}
	}

	return sum / (radius * radius * 4);
}
