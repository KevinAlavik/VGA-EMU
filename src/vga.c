#include "vga.h"

void log_message(const char *type, const char *message) {
  printf("[%s] %s\n", type, message);
}

void init_vram(VRAM *vram) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      int pos = y * WIDTH + x;
      vram->buffer[pos] = BLACK;
    }
  }
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

  vga.texture = SDL_CreateTexture(vga.renderer, SDL_PIXELFORMAT_ARGB8888,
                                  SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
  if (vga.texture == NULL) {
    log_message("ERROR", SDL_GetError());
    SDL_DestroyRenderer(vga.renderer);
    SDL_DestroyWindow(vga.window);
    SDL_Quit();
    exit(EXIT_FAILURE);
  }

  init_vram(&(vga.vram));
  vga.isAlive = 1;

  log_message("INFO", "VGA initialized");
  return vga;
}

void cleanup_vga(VGA *vga) {
  SDL_DestroyTexture(vga->texture);
  SDL_DestroyRenderer(vga->renderer);
  SDL_DestroyWindow(vga->window);
  SDL_Quit();
  log_message("INFO", "VGA cleaned up");
}

void update_texture(SDL_Texture *texture, VRAM *vram) {
  void *pixels;
  int pitch;
  SDL_LockTexture(texture, NULL, &pixels, &pitch);
  memcpy(pixels, vram->buffer, VRAM_SIZE * sizeof(uint32_t));
  SDL_UnlockTexture(texture);
}

void run_vga(VGA *vga) {
  static Uint32 lastFrameTime = 0;
  static Uint32 startTime = 0;
  Uint32 currentFrameTime = SDL_GetTicks();
  float deltaTime = (currentFrameTime - lastFrameTime) / 1000.0;
  lastFrameTime = currentFrameTime;

  if (startTime == 0) {
    startTime = currentFrameTime;
  }

  update_texture(vga->texture, &(vga->vram));

  SDL_SetRenderDrawColor(vga->renderer, 0, 0, 0, 255);
  SDL_RenderClear(vga->renderer);

  SDL_RenderCopy(vga->renderer, vga->texture, NULL, NULL);

  SDL_RenderPresent(vga->renderer);

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      vga->isAlive = 0;
    }
  }

  static int frames = 0;
  static float totalTime = 0.0;
  totalTime += deltaTime;
  frames++;

  if (totalTime >= 1.0) {
    float fps = frames / totalTime;
    Uint32 uptime = (currentFrameTime - startTime) / 1000;
    char title[100];
    sprintf(title, "VGA (%dx%d) FPS: %.2f Uptime: %02u:%02u", WIDTH, HEIGHT,
            fps, uptime / 60, uptime % 60);
    SDL_SetWindowTitle(vga->window, title);
    totalTime = 0.0;
    frames = 0;
  }

  int delayTime =
      (1000 / REFRESH_RATE_HZ) - (SDL_GetTicks() - currentFrameTime);
  if (delayTime > 0) {
    SDL_Delay(delayTime);
  }
}
