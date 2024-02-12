#include "vga.h"

void log_message(const char *type, const char *message) {
  printf("[%s] %s\n", type, message);
}

VGA init_vga() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    log_message("ERROR", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  VGA vga;

  char title[100];
  sprintf(title, "VGA (%dx%d)", WIDTH, HEIGHT);
  vga.window =
      SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  if (vga.window == NULL) {
    log_message("ERROR", SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
  }

  vga.renderer = SDL_CreateRenderer(vga.window, -1, SDL_RENDERER_ACCELERATED);
  if (vga.renderer == NULL) {
    log_message("ERROR", SDL_GetError());
    SDL_DestroyWindow(vga.window);
    SDL_Quit();
    exit(EXIT_FAILURE);
  }

  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      int pos = y * WIDTH + x;
      vga.vram.buffer[pos] = BLACK;
    }
  }
  vga.isAlive = 1;

  log_message("INFO", "VGA initialized");
  return vga;
}

void cleanup_vga(VGA *vga) {
  SDL_DestroyRenderer(vga->renderer);
  SDL_DestroyWindow(vga->window);
  SDL_Quit();
  log_message("INFO", "VGA cleaned up");
}

void draw_pixels(SDL_Renderer *renderer, VRAM *vram) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      int pos = y * WIDTH + x;
      Uint32 color = vram->buffer[pos];
      SDL_SetRenderDrawColor(renderer, (color >> 16) & 0xFF,
                             (color >> 8) & 0xFF, color & 0xFF, 255);
      SDL_RenderDrawPoint(renderer, x, y);
    }
  }
}

void init_vram(VRAM *vram) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      int pos = y * WIDTH + x;
      vram->buffer[pos] = BLACK;
    }
  }
}

void run_vga(VGA *vga) {
  int startTime = SDL_GetTicks(); // Get the current time

  SDL_SetRenderDrawColor(vga->renderer, 0, 0, 0, 255);
  SDL_RenderClear(vga->renderer);

  draw_pixels(vga->renderer, &(vga->vram));
  SDL_RenderPresent(vga->renderer);

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      vga->isAlive = 0;
    }
  }

  int endTime = SDL_GetTicks();
  int frameTime = endTime - startTime;
  float fps = 1000.0 / frameTime;

  char message[100];
  sprintf(message, "FPS: %.2f", fps);
  log_message("INFO", message);

  int delayTime = (1000 / REFRESH_RATE_HZ) - frameTime;
  if (delayTime > 0) {
    SDL_Delay(delayTime);
  }
}