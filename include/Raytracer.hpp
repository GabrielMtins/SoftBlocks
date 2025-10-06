#ifndef RAYCASTER_HPP
#define RAYCASTER_HPP

#include "Draw.hpp"
#include "Vec3.hpp"
#include "Vec2.hpp"
#include "GlobalDef.hpp"
#include "World.hpp"

#include <array>

struct Intersection {
	enum Type {
		TYPE_X,
		TYPE_Y,
		TYPE_Z
	};

	Vec3 position;
	Type type;
	int texture;
	float distance;

	bool not_found;

	Intersection(void);
	inline bool operator<(const Intersection& other) {
		if(not_found)
			return false;

		return distance < other.distance;
	}
};

class Raytracer {
	public:
		Raytracer(void);
		Draw& getScreen(void);
		void processScreen(void);
		void removeAimedBlock(void);
		void putAimedBlock(int type);

		struct {
			Vec3 position;
			Vec2 angle_x_direction;
			Vec2 angle_y_direction;
		} camera;

		World world;

	private:
		void processPixel(int x, int y);

		Intersection findClosestBlock(int x, int y) const;
		Intersection findClosestBlockX(const Vec3& direction) const;
		Intersection findClosestBlockY(const Vec3& direction) const;
		Intersection findClosestBlockZ(const Vec3& direction) const;
		Intersection findClosestBlockIterate(Vec3 start, const Vec3& delta) const;

		uint32_t interpolateColor(uint32_t c1, uint32_t c2, uint8_t part) const;

		bool checkIfBlockIsAimed(const Intersection& block) const;

		Draw screen;

		Draw textures;

		Intersection aimed_block;
};

#endif
