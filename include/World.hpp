#ifndef WORLD_HPP
#define WORLD_HPP

#include "GlobalDef.hpp"
#include "Draw.hpp"

#include <array>
#include <cstdint>

class World {
	public:
		World(void);
		uint8_t getBlock(int x, int y, int z) const;
		bool hasSolidBlock(int x, int y, int z) const;
		void setBlock(int x, int y, int z, int block);
		bool checkBoundaries(int x, int y, int z) const;
		void generate(void);
		int getHeight(int x, int z) const;

	private:
		int getIndex(int x, int y, int z) const;
		int getBlockType(int y, int height) const;
		void generateTree(const Draw& draw);
		uint32_t getAverage(const Draw& draw, int x, int y, int radius);

		std::array<uint8_t, WORLD_SIZE * WORLD_SIZE * WORLD_HEIGHT> blocks;
		Draw height_map;

	
};

#endif
