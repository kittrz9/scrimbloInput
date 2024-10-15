#ifndef KEYS_H
#define KEYS_H

#include <stdint.h>

#include <X11/Xlib.h>
#include <SDL3/SDL.h>

#define KEYS_INITIAL_SIZE 32

typedef struct {
	uint32_t x;
	uint32_t y;
	uint32_t textureOffset;
	KeyCode keycode;
} key;

extern key* keys;
extern size_t keyArraySize;
extern size_t keysLength;
extern uint16_t keySize;

void loadKeysImage(char* path);
void drawKeys(Display* disp, SDL_Surface* surface);

#endif // KEYS_H
