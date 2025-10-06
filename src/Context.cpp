#include "Context.hpp"
#include "GlobalDef.hpp"
#include <SDL2/SDL_image.h>

#include <cstdio>

Context::Context(void) {
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();

	window = SDL_CreateWindow(
			"SoftBlocks",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			WINDOW_WIDTH,
			WINDOW_HEIGHT,
			SDL_WINDOW_SHOWN
			);
	
	renderer = SDL_CreateRenderer(
			window,
			-1,
			SDL_RENDERER_ACCELERATED
			);

	SDL_SetRelativeMouseMode(SDL_TRUE);

	font = TTF_OpenFont("PublicPixel.ttf", 8);

	running = true;
}

void Context::pollEvents(void) {
	SDL_Event event;
	mouse_motion_x = 0;
	mouse_motion_y = 0;

	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_QUIT:
				running = false;
				break;

			case SDL_MOUSEMOTION:
				mouse_motion_x = event.motion.xrel;
				mouse_motion_y = event.motion.yrel;
				break;
		}
	}
}

bool Context::isRunning(void) const {
	return running;
}

void Context::updateWindowSurface(SDL_Surface *surface, int fps) {
	char str[256];
	sprintf(str, "%dx%d FPS: %d", SCREEN_WIDTH, SCREEN_HEIGHT, fps);
	SDL_Color color = {0xff, 0x00, 0x00, 0xff};
	SDL_Surface *surface_font = TTF_RenderText_Solid(font, str, color);
	SDL_BlitSurface(surface_font, NULL, surface, NULL);

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface_font);
}

Context::~Context(void) {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	IMG_Quit();
	SDL_Quit();
}
