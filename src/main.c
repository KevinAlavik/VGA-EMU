#include "vga.h"

#define SQUARE_SIZE 16

int main() {
  VGA vga = init_vga();

  while (vga.isAlive) {
    for (int y = 0; y < HEIGHT; ++y) {
      for (int x = 0; x < WIDTH; ++x) {
        Uint32 color =
            ((x / SQUARE_SIZE) + (y / SQUARE_SIZE)) % 2 ? WHITE : BLACK;
        vga.vram.buffer[y * WIDTH + x] = color;
      }
    }

    run_vga(&vga);
  }

  cleanup_vga(&vga);
  return 0;
}
