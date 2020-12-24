#include <SDL2/SDL.h>
#include <stdint.h>
#include <iostream>

#define DISPLAY_H

const unsigned int screenwidth = 64;
const unsigned int screenheight = 32;
const unsigned int gridsize = 10;

class Display{
	public:
		Display(void);
		void clear();
		bool drawSprite(void);

		
	protected:
		char screen[schreenheight][screenwidth];
		SDL_Window* window;
		SDL_Renderer* renderer;
};
