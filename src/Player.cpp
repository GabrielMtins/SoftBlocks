#include "Player.hpp"
#include "GlobalDef.hpp"
#include "Vec2.hpp"

Player::Player(void) {
	angle_y = 0.0f;
	angle_x = 0.0f;
	position = Vec3(WORLD_SIZE / 2, 140.0f, WORLD_SIZE / 2);
	size = Vec3(0.99f, 1.99f, 0.99f);
	on_floor = false;
	block_cooldown = 0;

	block_type = BLOCK_BRICKS;
}

void Player::handleInput(const Context& context, const World& world, float dt, Raytracer *raytracer) {
	const uint8_t *keys = SDL_GetKeyboardState(NULL);
	Vec3 wish_dir;
	Vec2 direction;
	uint32_t current_tick = SDL_GetTicks();
	uint32_t mouse_state = SDL_GetMouseState(NULL, NULL);

	(void) world;

	angle_y += MOUSE_SENSITIVITY * dt * context.mouse_motion_x;
	angle_x += MOUSE_SENSITIVITY * dt * context.mouse_motion_y;
	direction = Vec2(cosf(angle_y), sinf(angle_y));

	if(angle_x < - PI/2) {
		angle_x = -PI/2;
	}

	if(angle_x > PI/2) {
		angle_x = PI/2;
	}

	if(keys[SDL_SCANCODE_A]) {
		wish_dir.x--;
	}
	
	if(keys[SDL_SCANCODE_D]) {
		wish_dir.x++;
	}

	if(keys[SDL_SCANCODE_W]) {
		wish_dir.z++;
	}
	
	if(keys[SDL_SCANCODE_S]) {
		wish_dir.z--;
	}

	for(int i = 0; i < 9; i++) {
		if(keys[SDL_SCANCODE_1 + i]) {
			block_type = 1 + i;
		}
	}


	if(mouse_state & SDL_BUTTON(1)) {
		if(current_tick > block_cooldown) {
			block_cooldown = current_tick + PLAYER_BLOCK_COOLDOWN;
			raytracer->removeAimedBlock();
		}
	} 

	if(mouse_state & SDL_BUTTON(3)) {
		if(current_tick > block_cooldown) {
			block_cooldown = current_tick + PLAYER_BLOCK_COOLDOWN;
			raytracer->putAimedBlock(block_type);
		}
	} 

	if(!mouse_state) {
		block_cooldown = current_tick;
	}

	if(keys[SDL_SCANCODE_SPACE] && on_floor) {
		velocity.y = PLAYER_JUMP;
		on_floor = false;
	}

	velocity.x = 0.0f;
	velocity.z = 0.0f;

	if(wish_dir.lengthSqr() > 0.8f) {
		velocity += wish_dir.normalized().rotateY(direction.x, direction.y) * PLAYER_SPEED;
	}
}

bool Player::checkCollision(const World& world) {
	for(int i = 0; i < ceilf(size.x) + 1; i++) {
		for(int j = 0; j < ceilf(size.y) + 1; j++) {
			for(int k = 0; k < ceilf(size.z) + 1; k++) {
				float mx = floorf(position.x) + i;
				float my = floorf(position.y) + j;
				float mz = floorf(position.z) + k;

				if(!world.hasSolidBlock(mx, my, mz)) {
					continue;
				}

				if(position.x + size.x < mx)
					continue;

				if(position.y + size.y < my)
					continue;

				if(position.z + size.z < mz)
					continue;

				if(position.x > mx + 1.0f)
					continue;

				if(position.y > my + 1.0f)
					continue;

				if(position.z > mz + 1.0f)
					continue;

				return true;
			}
		}
	}

	return false;
}

void Player::applyMovement(const World& world, float dt) {
	Vec3 delta = velocity * dt;
	velocity.y += GRAVITY * dt;

	position.x += delta.x;

	if(checkCollision(world)) {
		position.x -= delta.x;
	}

	position.y += delta.y;

	if(checkCollision(world)) {
		position.y -= delta.y;

		if(velocity.y < 0.0f) {
			on_floor = true;
		}

		velocity.y = 0.0f;
	}

	position.z += delta.z;

	if(checkCollision(world)) {
		position.z -= delta.z;
	}
} 

Vec3 Player::getCenter(void) const {
	return 
		Vec3(
				position.x + size.x / 2,
				position.y + 1.65f,
				position.z + size.z / 2
			);
}
