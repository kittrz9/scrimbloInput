#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <X11/Xlib.h>

#include <SDL3/SDL.h>

#include "keys.h"
#include "config.h"

int main(int argc, char** argv) {
	if(argc < 2) {
		printf("no config file given\n");
		return 1;
	}
	if(!SDL_Init(SDL_INIT_VIDEO)) {
		printf("could not init sdl\n");
		return 1;
	}
	Display* disp = XOpenDisplay(NULL);
	loadConfig(disp, argv[1]);
	
	windowWidth *= keySize;
	windowHeight *= keySize;
	for(size_t i = 0; i < keysLength; ++i) {
		keys[i].x *= keySize;
		keys[i].y *= keySize;
		keys[i].textureOffset *= keySize;
	}

	SDL_Window* window = SDL_CreateWindow("scrimbloInput", windowWidth, windowHeight, 0);
	SDL_Surface* windowSurface = SDL_GetWindowSurface(window);


	SDL_Event e;
	uint8_t running = 1;
	while(running) {
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_EVENT_QUIT) {
				running = 0;
			}
		}
		drawKeys(disp, windowSurface);
		SDL_UpdateWindowSurface(window);
	}
	SDL_DestroyWindowSurface(window);
	SDL_DestroyWindow(window);
	free(keys);
}
