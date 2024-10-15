#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <X11/Xlib.h>

#include <SDL3/SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define MAX_STR_LEN 128

#define KEYS_INITIAL_SIZE 32

typedef struct {
	uint32_t x;
	uint32_t y;
	uint32_t textureOffset;
	KeyCode keycode;
} key;

uint16_t keySize;
uint16_t windowWidth;
uint16_t windowHeight;
char* keysImagePath;

uint8_t startsWith(char* str1, char* str2) {
	return strncmp(str1, str2, strlen(str2)) == 0;
}


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
	FILE* f = fopen(argv[1], "rb");
	key* keys = malloc(sizeof(key) * KEYS_INITIAL_SIZE);
	size_t keysLength = KEYS_INITIAL_SIZE;
	size_t keysIndex = 0;
	char str[MAX_STR_LEN];
	fgets(str, MAX_STR_LEN, f);
	if(!startsWith(str, "SCRIMBLO_INPUT_FORMAT_V0")) {
		printf("invalid config file\n");
		return 1;
	}
	while(fgets(str, MAX_STR_LEN, f)) {
		printf("%s\n", str);
		if(*str == '#' || *str == '\n') {
			continue;
		}
		if(startsWith(str, "KEY_SIZE")) {
			keySize = atoi(str+9);
			printf("key size: %i\n", keySize);
			continue;
		}
		if(startsWith(str, "KEYS_IMAGE")) {
			str[strlen(str)-1] = '\0';
			keysImagePath = malloc(strlen(str+11));
			strcpy(keysImagePath, str+11);
			continue;
		}
		if(startsWith(str, "KEY_START")) {
			while(fgets(str, MAX_STR_LEN, f)){
				if(startsWith(str, "KEY_END")) {
					break;
				}
				if(startsWith(str, "X")) {
					keys[keysIndex].x = atoi(str+2);
					if(keys[keysIndex].x+1 > windowWidth) { windowWidth = keys[keysIndex].x+1; }
					printf("- KEY %i X %i\n", keysIndex, keys[keysIndex].x);
				}
				if(startsWith(str, "Y")) {
					keys[keysIndex].y = atoi(str+2);
					if(keys[keysIndex].y+1 > windowHeight) { windowHeight = keys[keysIndex].y+1; }
					printf("- KEY %i Y %i\n", keysIndex, keys[keysIndex].y);
				}
				if(startsWith(str, "KEYSYM")) {
					str[strlen(str)-1] = '\0';
					int keysym = XStringToKeysym(str+7);
					printf("%s, %i\n", str, keysym);
					keys[keysIndex].keycode = XKeysymToKeycode(disp, keysym);
				}
				if(startsWith(str, "OFFSET")) {
					keys[keysIndex].textureOffset = atoi(str+7);
					printf("- KEY %i OFFSET %i\n", keysIndex, keys[keysIndex].textureOffset);
				}
			}
			++keysIndex;
			if(keysIndex >= keysLength) {
				keysLength *= 2;
				keys = realloc(keys, sizeof(key) * keysLength);
			}
			continue;
		}
		printf("unknown config instruction %s\n", str);
	}
	fclose(f);

	int imageW;
	int imageH;
	int imageC;
	uint8_t* imageData = stbi_load(keysImagePath, &imageW, &imageH, &imageC, 4);
	if(imageData == NULL) {
		printf("could not load image \"%s\"\n", keysImagePath);
		free(keys);
		return 1;
	}
	SDL_Surface* imageSurface = SDL_CreateSurface(imageW, imageH, SDL_PIXELFORMAT_RGBA32);
	uint8_t* surfacePtr = imageSurface->pixels;
	uint8_t* dataPtr = imageData;
	for(uint32_t y = 0; y < imageH; ++y) {
		memcpy(surfacePtr, dataPtr, imageW*4);
		surfacePtr += imageSurface->pitch;
		dataPtr += imageW*4;
	}
	
	windowWidth *= keySize;
	windowHeight *= keySize;
	for(size_t i = 0; i < keysLength; ++i) {
		keys[i].x *= keySize;
		keys[i].y *= keySize;
		keys[i].textureOffset *= keySize;
	}

	SDL_Window* window = SDL_CreateWindow("scrimbloInput", windowWidth, windowHeight, 0);
	SDL_Surface* windowSurface = SDL_GetWindowSurface(window);

	char keymap[32];

	SDL_Event e;
	uint8_t running = 1;
	while(running) {
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_EVENT_QUIT) {
				running = 0;
			}
		}
		XQueryKeymap(disp, keymap);
		for(size_t i = 0; i < keysIndex; ++i) {
			KeyCode k = keys[i].keycode;
			uint8_t keyState = (keymap[k/8] & (1 << k%8)) != 0;
			SDL_Rect srcRect = {
				.x = keys[i].textureOffset,
				.y = keySize * keyState,
				.w = keySize,
				.h = keySize,
			};
			SDL_Rect dstRect = {
				.x = keys[i].x,
				.y = keys[i].y,
				.w = keySize,
				.h = keySize,
			};
			SDL_BlitSurface(imageSurface, &srcRect, windowSurface, &dstRect);
		}
		SDL_UpdateWindowSurface(window);
	}
	SDL_DestroySurface(imageSurface);
	SDL_DestroyWindowSurface(window);
	SDL_DestroyWindow(window);
	free(keys);
}
