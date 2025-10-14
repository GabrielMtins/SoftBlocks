#include "Context.hpp"
#include "Draw.hpp"
#include "Vec2.hpp"
#include "Raytracer.hpp"
#include "Player.hpp"
#include "Vec3.hpp"

#include <ctime>
#include <cstdlib>
#include <cmath>

#include <omp.h>

const Vec2 Vec2::zero = Vec2(0.0f, 0.0f);

static void run_args(int argc, char **argv) {
	for(int i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "--single")) {
			omp_set_num_threads(1);
		}
	}
}

/**
 * A função main que possui todas as variáveis e objetos para
 * funcionar. 
 * Se quiser saber como o raycasting em si funciona, basta ler
 * o Raycaster.hpp e o Raycaster.cpp
 */
int main(int argc, char **argv) {
	uint32_t start_tick;
	float fps = 60.0f;
	uint32_t next_tick_fps;
	float dt = 0.00f;;
	srand(time(NULL));
	//srand(0);
	//srand(172);

	run_args(argc, argv);

	uint32_t final_tick;
	uint32_t counter = 0;

	Context context;
	Raytracer *raytracer = new Raytracer();
	Player player;
	uint32_t first_tick = SDL_GetTicks();
	next_tick_fps = 0;

	while(context.isRunning()) {
		start_tick = SDL_GetTicks();

		/**
		 * Atualiza os eventos, input do player, e
		 * renderiza para a tela... */
		context.pollEvents();
		player.handleInput(context, raytracer->world, dt, raytracer);
		player.applyMovement(raytracer->world, dt);

		/**
		 * Atualizar as posições da câmera e dos ângulos
		 * segundo a posição do player */
		raytracer->camera.position = player.getCenter();
		raytracer->camera.angle_y_direction = Vec2(cosf(player.angle_y), sinf(player.angle_y));
		raytracer->camera.angle_x_direction = Vec2(cosf(player.angle_x), sinf(player.angle_x));

		/* Renderizar na tela */
		raytracer->processScreen();
		context.updateWindowSurface(raytracer->getScreen().getSurface(), fps);

		dt = (float) (SDL_GetTicks() - start_tick) / 1000;

		if(start_tick > next_tick_fps) {
			fps = 1.0f / dt;
			next_tick_fps = start_tick + 100;
		}
		
		counter++;
	}

	final_tick = SDL_GetTicks();
	float delta = (float) (final_tick - first_tick) / 1000.0f;

	printf("Program time: %f\n", delta);
	printf("Total frames: %u\n", counter);
	printf("Average FPS: %f\n", (float) counter / delta);

	return 0;
}
