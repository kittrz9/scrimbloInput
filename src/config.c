#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "keys.h"

#define MAX_STR_LEN 128

uint16_t windowWidth;
uint16_t windowHeight;

uint8_t startsWith(char* str1, char* str2) {
	return strncmp(str1, str2, strlen(str2)) == 0;
}

void loadConfig(Display* disp, char* path) {
	FILE* f = fopen(path, "rb");
	if(f == NULL) {
		printf("could not open config file \"%s\"\n", path);
		exit(1);
	}
	char str[MAX_STR_LEN];
	char* keysImagePath;
	keys = malloc(sizeof(key) * KEYS_INITIAL_SIZE);
	fgets(str, MAX_STR_LEN, f);
	if(!startsWith(str, "SCRIMBLO_INPUT_FORMAT_V0")) {
		printf("invalid config file\n");
		exit(1);
	}
	while(fgets(str, MAX_STR_LEN, f)) {
		if(*str == '#' || *str == '\n') {
			continue;
		}
		if(startsWith(str, "KEY_SIZE")) {
			keySize = atoi(str+9);
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
					keys[keysLength].x = atoi(str+2);
					if(keys[keysLength].x+1 > windowWidth) { windowWidth = keys[keysLength].x+1; }
				}
				if(startsWith(str, "Y")) {
					keys[keysLength].y = atoi(str+2);
					if(keys[keysLength].y+1 > windowHeight) { windowHeight = keys[keysLength].y+1; }
				}
				if(startsWith(str, "KEYSYM")) {
					str[strlen(str)-1] = '\0';
					int keysym = XStringToKeysym(str+7);
					keys[keysLength].keycode = XKeysymToKeycode(disp, keysym);
				}
				if(startsWith(str, "OFFSET")) {
					keys[keysLength].textureOffset = atoi(str+7);
				}
			}
			++keysLength;
			if(keysLength >= keyArraySize) {
				keyArraySize *= 2;
				keys = realloc(keys, sizeof(key) * keyArraySize);
			}
			continue;
		}
		printf("unknown config instruction %s\n", str);
	}
	fclose(f);
	loadKeysImage(keysImagePath);
	free(keysImagePath);
}
