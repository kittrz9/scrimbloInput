#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#include <X11/Xlib.h>

extern uint16_t windowWidth;
extern uint16_t windowHeight;

void loadConfig(Display* disp, char* path);

#endif // CONFIG_H
