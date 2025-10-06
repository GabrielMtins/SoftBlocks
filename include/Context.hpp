#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/**
 * Classe base, apenas para criar uma janela
 * e atualizar os eventos que ocorrem nela. Estaremos
 * usando o SDL2 para isso.
 */
class Context {
	public:
		Context(void);

		void pollEvents(void);

		bool isRunning(void) const;

		void updateWindowSurface(SDL_Surface *surface, int fps);

		~Context(void);

		int mouse_motion_x;
		int mouse_motion_y;

	private:
		SDL_Window *window;
		SDL_Renderer *renderer;
		bool running;
		TTF_Font *font;
};

#endif
