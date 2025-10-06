#ifndef GLOBALDEF_HPP
#define GLOBALDEF_HPP

/**
 * Definições globais do raycaster,
 * como tamanho da janela e o tamanho da tela a ser desenhada.
 */

#define WINDOW_WIDTH 320 * 4
#define WINDOW_HEIGHT 240 * 4

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define WORLD_SIZE 512
#define WORLD_HEIGHT 256

#define NUM_TREE 4096

#define FOG_DISTANCE_SQR 3200

#define FOG_COLOR 0xdef0ffff

#define WHITE 0xffffffff
#define BLACK 0x000000ff

#define WATER_COLOR 0x0000ffff
#define WATER_LEVEL 122.8f

#define WORLD_SMOOTHNESS 8
#define WORLD_RADIUS_SMOOTH 4

#define PI 3.1415f

#define PLAYER_JUMP 8.0f
#define GRAVITY -10.0f
#define PLAYER_AIM_DISTANCE_SQR 36.0f

#define PLAYER_BLOCK_COOLDOWN 200

#define PLAYER_SPEED 4.0f
#define MOUSE_SENSITIVITY 0.5f
#define TURN_SPEED 4.0f

enum Blocks {
	BLOCK_AIR = 0,
	BLOCK_GRASS,
	BLOCK_DIRT,
	BLOCK_STONE,
	BLOCK_SAND,
	BLOCK_WOOD,
	BLOCK_LEAF,
	BLOCK_COBBLESTONE,
	BLOCK_PLANKS,
	BLOCK_BRICKS,
};

#endif
