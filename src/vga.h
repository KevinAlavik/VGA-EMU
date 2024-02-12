#ifndef __VGA_H__
#define __VGA_H__

#include <SDL2/SDL.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 640
#define HEIGHT 480

#define VRAM_SIZE (WIDTH * HEIGHT)
#define REFRESH_RATE_HZ 60

typedef enum {
  BLACK = 0x000000,
  BLUE = 0x0000FF,
  GREEN = 0x00FF00,
  CYAN = 0x00FFFF,
  RED = 0xFF0000,
  MAGENTA = 0xFF00FF,
  BROWN = 0xA52A2A,
  LIGHT_GRAY = 0xD3D3D3,
  DARK_GRAY = 0xA9A9A9,
  LIGHT_BLUE = 0xADD8E6,
  LIGHT_GREEN = 0x90EE90,
  LIGHT_CYAN = 0xE0FFFF,
  LIGHT_RED = 0xFF6347,
  LIGHT_MAGENTA = 0xFFB6C1,
  YELLOW = 0xFFFF00,
  WHITE = 0xFFFFFF
} ColorPalette;

typedef struct {
  uint32_t buffer[VRAM_SIZE];
} VRAM;

typedef struct {
  VRAM vram;
  int isAlive;
  SDL_Window *window;
  SDL_Renderer *renderer;
} VGA;

VGA init_vga();
void init_vram(VRAM *vram);
void run_vga(VGA *vga);
void cleanup_vga(VGA *vga);

#endif // __VGA_H__
