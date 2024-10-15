#include "keys.h"

#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <SDL3/SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

key* keys;
size_t keyArraySize = KEYS_INITIAL_SIZE;
size_t keysLength = 0;
uint16_t keySize;

char keymap[32];

SDL_Surface* keysImage;

void loadKeysImage(char* path) {
	int imageW;
	int imageH;
	int imageC;
	uint8_t* imageData = stbi_load(path, &imageW, &imageH, &imageC, 4);
	if(imageData == NULL) {
		printf("could not load image \"%s\"\n", path);
		return;
	}
	keysImage = SDL_CreateSurface(imageW, imageH, SDL_PIXELFORMAT_RGBA32);
	uint8_t* surfacePtr = keysImage->pixels;
	uint8_t* dataPtr = imageData;
	for(uint32_t y = 0; y < imageH; ++y) {
		memcpy(surfacePtr, dataPtr, imageW*4);
		surfacePtr += keysImage->pitch;
		dataPtr += imageW*4;
	}
}

void drawKeys(Display* disp, SDL_Surface* surface) {
	XQueryKeymap(disp, keymap);
	for(size_t i = 0; i < keysLength; ++i) {
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
		SDL_BlitSurface(keysImage, &srcRect, surface, &dstRect);
	}
}
