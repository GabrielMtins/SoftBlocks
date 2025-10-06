#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Context.hpp"
#include "Vec3.hpp"
#include "Draw.hpp"
#include "World.hpp"
#include "Raytracer.hpp"

/**
 * Estrutura de Player, apenas responsável pela movimentação e 
 * colisão com o mundo, além de input. Não possui sistema de movimentação
 * avançado, pois não é o foco desse projeto.
 */
struct Player {
	Vec3 position;
	Vec3 velocity;
	Vec3 size;
	bool on_floor;

	float angle_y, angle_x;
	uint32_t block_cooldown;

	int block_type;

	Player(void);

	void handleInput(const Context& context, const World& world, float dt, Raytracer *raytracer);

	bool checkCollision(const World& world);
	
	void applyMovement(const World& world, float dt);

	Vec3 getCenter(void) const;
};

#endif
