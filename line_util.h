// CS4 Graphics Practical 1
//
// Rebecca Cran <brucec@dcs.ed.ac.uk>

// This is my implementation of the classis bresenham algorithm.
// The simple midpoint algorithm code has been optimized to remove
// floating-point calculations.
//

#ifndef _LINE_UTIL_H_
#define _LINE_UTIL_H_

#include <unistd.h>
#include <stdlib.h>

extern "C" {
#include <SDL.h>
}

#include "file_parser.h"

struct triangle
{
	vertex v1;
	vertex v2;
	vertex v3;
	bool visible;
};

struct point {
	int x;
	int y;
	int z;
};

struct tri_iter_s
{
	int curr_item;
};

typedef struct tri_iter_s tri_iter;

struct line_3d
{
	vertex a;
	vertex b;
};

struct color
{
	int r;
	int g;
	int b;
};


static int grid_size = 1024;

line_3d make_line(float x1, float y1, float z1, float x2, float y2, float z2);
const void put_pixel(point);
void putpixelpoint(int x, int y, int pixelval);
void draw_grid();
void draw_line(point, point);
point get_logical_pos(point);
void clear_to_grid();
void update_framebuffer();
int calc_color(int, int);
int calc_color_inv(int, int);
const void redraw_screen();
const inline void draw_rubber_band(point a, point b);
void setup_grayscale_table();
void set_color(int color);
SDL_Surface * sdl_init();
void clear_screen();

void handle_events();
const void put_pixel(point a);
SDL_Surface  * init_graphics();
void set_color(int color);
void drawPolygon(point points[3], int color);
void draw_line_antialiased_wu(point a, point b);
void draw_line_bresenham(point a, point b);

void poly_start();
void poly_end();
void poly_vertex(float x, float y, float z);


void tri_clear();
void tri_add(triangle t);
void tri_remove(triangle t);
bool tri_compare(triangle t1, triangle t2);
triangle * tri_get(int item);
tri_iter * tri_init_iter();
triangle * tri_get_next();
void tri_render();
bool isvisible(triangle t);
void scale_and_clip(point p[3], point r[3]);

#endif
