#include "Raytracer.hpp"
#include <cmath>
#include <array>
#include <algorithm>

#include <omp.h>

#define RMASK(x) ((x >> 24) & 0xff)
#define GMASK(x) ((x >> 16) & 0xff)
#define BMASK(x) ((x >> 8) & 0xff)
#define AMASK(x) ((x) & 0xff)

#define PART(x, y, part) (((x * part) >> 8) + ((y * (0xff - part)) >> 8))

Intersection::Intersection(void) {
	type = TYPE_X;
	texture = 0;
	distance = 0.0f;
	not_found = false;
}

Raytracer::Raytracer(void) {
	screen.createScreen(SCREEN_WIDTH, SCREEN_HEIGHT);
	textures.load("textures.png");

	camera.position = Vec3(5.0f, 1.62f, 2.0f);

	world.generate();
}

Draw& Raytracer::getScreen(void) {
	return screen;
}

void Raytracer::processScreen(void) {
	int i, j;

	screen.clear(FOG_COLOR);

	aimed_block = findClosestBlock(
			SCREEN_WIDTH / 2,
			SCREEN_HEIGHT / 2
			);

	#pragma omp parallel for collapse(2) private(i) schedule(dynamic) 
	for(j = 0; j < SCREEN_HEIGHT; j++) {
		for(i = 0; i < SCREEN_WIDTH; i++) {
			processPixel(i, j);
		}
	}
}

void Raytracer::removeAimedBlock(void) {
	if(aimed_block.distance > PLAYER_AIM_DISTANCE_SQR)
		return;

	world.setBlock(
			aimed_block.position.x,
			aimed_block.position.y,
			aimed_block.position.z,
			0
			);
}

void Raytracer::putAimedBlock(int type) {
	int map_x, map_y, map_z;

	if(aimed_block.distance > PLAYER_AIM_DISTANCE_SQR)
		return;

	Vec3& position = aimed_block.position;

	map_x = position.x;
	map_y = position.y;
	map_z = position.z;

	Vec3 middle = Vec3(
			floorf(position.x) + 0.5f,
			floorf(position.y) + 0.5f,
			floorf(position.z) + 0.5f
			);

	switch(aimed_block.type) {
		case Intersection::TYPE_X:
			if(position.x < middle.x) {
				map_x--;
			} else {
				map_x++;
			}
			break;

		case Intersection::TYPE_Y:
			if(position.y < middle.y) {
				map_y--;
			} else {
				map_y++;
			}
			break;

		case Intersection::TYPE_Z:
			if(position.z < middle.z) {
				map_z--;
			} else {
				map_z++;
			}
			break;
	}

	world.setBlock(
			map_x,
			map_y,
			map_z,
			type
			);
}

void Raytracer::processPixel(int x, int y) {
	Intersection intersection = findClosestBlock(x, y);
	Vec3& position = intersection.position;

	int texture_x = 0, texture_y = 0;
	uint32_t part, color;

	if(intersection.not_found)
		return;

	if(intersection.distance > FOG_DISTANCE_SQR)
		return;

	switch(intersection.type) {
		case Intersection::TYPE_X:
			texture_y = (position.y - floorf(position.y)) * textures.getWidth();
			texture_x = (position.z - floorf(position.z)) * textures.getWidth();
			break;

		case Intersection::TYPE_Y:
			texture_y = (position.x - floorf(position.x)) * textures.getWidth();
			texture_x = (position.z - floorf(position.z)) * textures.getWidth();
			break;

		case Intersection::TYPE_Z:
			texture_x = (position.x - floorf(position.x)) * textures.getWidth();
			texture_y = (position.y - floorf(position.y)) * textures.getWidth();
			break;
	}


	part = 255 * (FOG_DISTANCE_SQR - intersection.distance) / FOG_DISTANCE_SQR;
	if(part > 255)
		part = 255;

	texture_y += intersection.texture * textures.getWidth();
	color = textures.getPixel(texture_x, texture_y);

	switch(intersection.type) {
		case Intersection::TYPE_X:
			color = interpolateColor(color, BLACK, 220);
			break;

		case Intersection::TYPE_Z:
			color = interpolateColor(color, BLACK, 180);
			break;

		default:
			break;
	}

	if(position.y < WATER_LEVEL && position.y > world.getHeight(position.x, position.z) - 1.0f) {
		color = interpolateColor(color, WATER_COLOR, 150);
	}
	/*
	if(position.y < WATER_LEVEL) {
		color = interpolateColor(color, WATER_COLOR, 150);
	}
	*/

	if(checkIfBlockIsAimed(intersection)) {
		color = interpolateColor(color, WHITE, 100);
	}

	color = interpolateColor(color, FOG_COLOR, part);

	screen.putPixel(
			x,
			y,
			color
			);
}

Intersection Raytracer::findClosestBlock(int x, int y) const {
	Intersection intersection;

	Vec3 direction = Vec3(
			(float) (x - SCREEN_WIDTH / 2) / SCREEN_HEIGHT,
			(float) -(y - SCREEN_HEIGHT / 2) / SCREEN_HEIGHT,
			0.5f
			);

	direction = direction.rotateX(camera.angle_x_direction.x, camera.angle_x_direction.y);
	direction = direction.rotateY(camera.angle_y_direction.x, camera.angle_y_direction.y);

	std::array<Intersection, 3> intersections;

	intersections[0] = findClosestBlockX(direction);
	intersections[1] = findClosestBlockY(direction);
	intersections[2] = findClosestBlockZ(direction);

	std::sort(intersections.begin(), intersections.end());

	for(auto& i : intersections) {
		i.texture = world.getBlock(
				i.position.x,
				i.position.y,
				i.position.z
				) - 1;

		if(!i.not_found)
			return i;
	}

	return intersections[0];
}

Intersection Raytracer::findClosestBlockX(const Vec3& direction) const {
	Intersection intersection;

	Vec3 start = camera.position;
	Vec3 first_delta;
	Vec3 delta;

	if(direction.x == 0.0f) {
		intersection.not_found = true;
		return intersection;
	}

	if(direction.x > 0.0f) {
		first_delta.x = floorf(start.x + 1) - start.x;
	} else {
		first_delta.x = floorf(start.x) - start.x;
	}

	first_delta.y = direction.y / direction.x * first_delta.x;
	first_delta.z = direction.z / direction.x * first_delta.x;

	start += first_delta;

	delta = direction / fabsf(direction.x);

	intersection = findClosestBlockIterate(start, delta);
	intersection.type = Intersection::TYPE_X;
	
	return intersection;
}

Intersection Raytracer::findClosestBlockY(const Vec3& direction) const {
	Intersection intersection;
	Vec3 start = camera.position;
	Vec3 first_delta;
	Vec3 delta;

	if(direction.y == 0.0f) {
		intersection.not_found = true;
		return intersection;
	}

	if(direction.y > 0.0f) {
		first_delta.y = floorf(start.y + 1) - start.y;
	} else {
		first_delta.y = floorf(start.y) - start.y;
	}

	first_delta.x = direction.x / direction.y * first_delta.y;
	first_delta.z = direction.z / direction.y * first_delta.y;

	start += first_delta;

	delta = direction / fabsf(direction.y);
	
	intersection = findClosestBlockIterate(start, delta);
	intersection.type = Intersection::TYPE_Y;

	return intersection;
}

Intersection Raytracer::findClosestBlockZ(const Vec3& direction) const {
	Intersection intersection;
	Vec3 start = camera.position;
	Vec3 first_delta;
	Vec3 delta;

	if(direction.z == 0.0f) {
		intersection.not_found = true;
		return intersection;
	}

	if(direction.z > 0.0f) {
		first_delta.z = floorf(start.z + 1) - start.z;
	} else {
		first_delta.z = floorf(start.z) - start.z;
	}

	first_delta.x = direction.x / direction.z * first_delta.z;
	first_delta.y = direction.y / direction.z * first_delta.z;

	start += first_delta;

	delta = direction / fabsf(direction.z);

	intersection = findClosestBlockIterate(start, delta);
	intersection.type = Intersection::TYPE_Z;

	return intersection;
}

Intersection Raytracer::findClosestBlockIterate(Vec3 start, const Vec3& delta) const {
	Intersection intersection;

	if(delta.x < 0.0f) {
		start.x -= 0.001f;
	}

	if(delta.y < 0.0f) {
		start.y -= 0.001f;
	}

	if(delta.z < 0.0f) {
		start.z -= 0.001f;
	}

	intersection.distance = 0.0f;

	while(1) {
		int map_x, map_y, map_z;
		intersection.distance = (start - camera.position).lengthSqr();

		map_x = start.x;
		map_y = start.y;
		map_z = start.z;

		if(intersection.distance > FOG_DISTANCE_SQR) {
			break;
		}

		if(!world.checkBoundaries(map_x, map_y, map_z)) {
			intersection.not_found = true;
			break;
		}

		if(world.hasSolidBlock(map_x, map_y, map_z)) {
			break;
		}

		start += delta;
	}

	intersection.position = start;

	return intersection;
}

uint32_t Raytracer::interpolateColor(uint32_t c1, uint32_t c2, uint8_t part) const {
	uint32_t r, g, b, a;
	uint32_t color;

	r = PART(RMASK(c1), RMASK(c2), part);
	g = PART(GMASK(c1), GMASK(c2), part);
	b = PART(BMASK(c1), BMASK(c2), part);
	a = PART(AMASK(c1), AMASK(c2), part);

	color = (r << 24) | (g << 16) | (b << 8) | a;

	return color;
}

bool Raytracer::checkIfBlockIsAimed(const Intersection& block) const {
	if(aimed_block.distance > PLAYER_AIM_DISTANCE_SQR)
		return false;

	if(block.type != aimed_block.type)
		return false;

	if(int(block.position.x) != int(aimed_block.position.x))
		return false;

	if(int(block.position.y) != int(aimed_block.position.y))
		return false;

	if(int(block.position.z) != int(aimed_block.position.z))
		return false;

	return true;
}
