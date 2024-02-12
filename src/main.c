#include "vga.h"
#include <math.h>

#define PI 3.14159265
#define CUBE_SIZE 60
#define ROTATION_SPEED 0.05

typedef struct {
  double x, y, z;
} V3D;
void po(V3D *v, int *x, int *y) {
  *x = (int)(WIDTH / 2 + v->x);
  *y = (int)(HEIGHT / 2 + v->y);
}
void r_x(V3D *v, double a) {
  double c = cos(a), s = sin(a), ny = v->y * c - v->z * s,
         nz = v->y * s + v->z * c;
  v->y = ny;
  v->z = nz;
}
void r_y(V3D *v, double a) {
  double c = cos(a), s = sin(a), nx = v->x * c + v->z * s,
         nz = -v->x * s + v->z * c;
  v->x = nx;
  v->z = nz;
}
void l(int x0, int y0, int x1, int y1, Uint32 c, VRAM *v) {
  int dx = abs(x1 - x0), dy = abs(y1 - y0), sx = x0 < x1 ? 1 : -1,
      sy = y0 < y1 ? 1 : -1, e = dx - dy;
  while (x0 != x1 || y0 != y1) {
    if (x0 >= 0 && x0 < WIDTH && y0 >= 0 && y0 < HEIGHT)
      v->buffer[y0 * WIDTH + x0] = c;
    int e2 = 2 * e;
    if (e2 > -dy) {
      e -= dy;
      x0 += sx;
    }
    if (e2 < dx) {
      e += dx;
      y0 += sy;
    }
  }
}
void c(double a, VRAM *v) {
  V3D p[8] = {
      {-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE}, {CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE},
      {CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE},   {-CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE},
      {-CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE},  {CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE},
      {CUBE_SIZE, CUBE_SIZE, CUBE_SIZE},    {-CUBE_SIZE, CUBE_SIZE, CUBE_SIZE}};
  for (int i = 0; i < 8; ++i) {
    r_x(&p[i], a);
    r_y(&p[i], a);
  }
  int e[12][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6},
                  {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};
  for (int i = 0; i < 12; ++i) {
    int x0, y0, x1, y1;
    po(&p[e[i][0]], &x0, &y0);
    po(&p[e[i][1]], &x1, &y1);
    l(x0, y0, x1, y1, 0xFFFFFF, v);
  }
}
int main() {
  VGA v = init_vga();
  double a = 0.0;
  while (v.isAlive) {
    memset(v.vram.buffer, 0, sizeof(Uint32) * WIDTH * HEIGHT);
    c(a, &(v.vram));
    a += ROTATION_SPEED;
    if (a >= 2 * PI)
      a -= 2 * PI;
    run_vga(&v);
  }
  cleanup_vga(&v);
  return 0;
}
