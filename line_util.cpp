// Computer Science 4:  Computer Graphics
//
// Bruce Cran, brucec@dcs.ed.ac.uk
//
// This is a line-drawing program utilising the Simple Raster Graphics Package, SRGP
// It demonstates both the famous Bresenham line-drawing algorithm and the less well-known Wu's method of anti-aliasing lines drawn using Bresenham's algorithm
//
// Ideas for the Bresenham algorithm were from 	http://www.cs.unc.edu/~mcmillan/comp136/Lecture6/Lines.html
// and for Wu's algorithm:                     	http://freespace.virgin.net/hugo.elias/graphics/x_wuline.htm,
// 						http://www.whisqu.se/per/docs/graphics75.htm


#include <unistd.h>
#include <stdlib.h>

// SRGP is written in C, so don't mangle function names

extern "C" {
#include <SDL.h>
}

// for floor, sqrt functions
#include <math.h>
#include <vector>
#include <SDL_gfxPrimitives.h>

#include "line_util.h"
#include "file_parser.h"
#include "render.h"
#include "manip.h"

using namespace std;

const int COLOR_BLACK = 0xffff; // inverted colors
const int COLOR_WHITE = 0x0000; // inverted colors

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 1024;
const int BITS_PER_PIXEL = 16;
const int MAX_COLOR_VALUE = 255;

static const bool antialiased = false;
static bool do_draw_grid   = false;
//static bool simple_line = true;


// calculated from pixels_per_point: how many physical pixels are there
// for every logical pixel?
static int pixels_per_point = 80;

// user-definable: how many pixels do you want across the screen?

// float-buffer the images for speed and rubber-banding
static SDL_Surface * frame_buffer;
static SDL_Surface * back_buffer;
static char 	   * z_buffer;

//static int num_vert;
//static vertex v[64];

static int pixelbuf_size;
static int current_color = 0xffff;

line_3d make_line(float x1,float y1, float z1, float x2, float y2, float z2)
{
	vertex p1 = {x1,y1,z1};
	vertex p2 = {x2,y2,z2};
	line_3d l = {p1,p2};
	return l;
}

SDL_Surface * init_graphics()
{
	SDL_Surface * surface = (SDL_Surface*) sdl_init();
	return surface;
}

SDL_Surface * sdl_init()
{
	if(SDL_Init(SDL_INIT_VIDEO) == -1) {
		printf("SDL_Init failed: %s", SDL_GetError());
		return 0;
	}
	
	frame_buffer = SDL_SetVideoMode(WINDOW_WIDTH,WINDOW_HEIGHT,BITS_PER_PIXEL, SDL_SWSURFACE);
	if(frame_buffer == NULL) {
		printf("Could not initialize SDL");
		return 0;
	} else {
		back_buffer = (SDL_Surface*) malloc(sizeof(SDL_Surface));
		pixelbuf_size = frame_buffer->h * frame_buffer->pitch + frame_buffer->w *
				frame_buffer->format->BytesPerPixel;
		memcpy(back_buffer, frame_buffer, sizeof(SDL_Surface));
		char * bb_pixels = (char*) malloc(pixelbuf_size);
		back_buffer->pixels = bb_pixels;
		z_buffer = (char*) malloc(pixelbuf_size);
		memset(z_buffer, 0, pixelbuf_size);
	}
	return back_buffer;
}

void handle_events()
{
	SDL_Event event;

	while(true) {
		SDL_WaitEvent(&event);
		if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_MIDDLE)
			break;
		switch(event.type) {
		}
	}
}

// draw a line from a to b.  set antialiased to 'true' in p1.h to draw using antialiased lines, 'false' to use
// just plain bresenham lines with no antialiasing
void draw_line(point a, point b)
{
	if (antialiased)
		draw_line_antialiased_wu(a, b);
	else
		draw_line_bresenham(a, b);
}

// this is supposed to redraw the screen in an optimal manner.  TODO: add clipping rectangles to only copy
// the parts of the screen which have changed, between the frame and back buffers
const void redraw_screen()
{
    update_framebuffer();
}

// copies the contents of the backbuffer to the framebuffer, obliterating anything which was there
// TODO: calculate which bits of the screen actually need copies?  or is memcpy so efficient that it's faster
// just copying the whole lot across?
void update_framebuffer()
{
	memcpy(frame_buffer->pixels, back_buffer->pixels, pixelbuf_size);
	SDL_UpdateRect(frame_buffer, 0, 0, 1024, 768);	
}


// draw a temporary 'rubber-band' line in the framebuffer.  This just
// uses the SRGP_line and is just to allow the user to see where they're putting
// the line I'm going to draw when they let go of the button
const void draw_rubber_band(point a, point b)
{
#if 0
    point scaled_start = { a.x * pixels_per_point + (pixels_per_point / 2),
	a.y * pixels_per_point + (pixels_per_point / 2)
    }, scaled_end = {
    b.x * pixels_per_point + (pixels_per_point / 2),
	    b.y * pixels_per_point + (pixels_per_point / 2)};
#endif
}


// draw a grid of logical pixels on the screen
void draw_grid()
{
#if 0
    int x = 0, y = 0;
    SRGP_setColor(1);

    for (x = 0; x <= WINDOW_WIDTH; x += pixels_per_point) {
	SRGP_lineCoord(x, y, x, y + WINDOW_HEIGHT);
    }

    x = 0;
    for (y = 0; y <= WINDOW_HEIGHT; y += pixels_per_point) {
	SRGP_lineCoord(x, y, x + WINDOW_WIDTH, y);
    }
    
#endif
}

// convert the physical pixel location physpoint
// into a logical pixel defined by the grid_size variable
point get_logical_pos(point physpoint)
{
    point logpix;
    logpix.x = (physpoint.x / pixels_per_point);
    logpix.y = (physpoint.y / pixels_per_point);
    return logpix;
}

// clear the screen, erasing any lines drawn, and redraw the pixel grid.
// Then, put this into the framebuffer so the user can see it
void clear_to_grid()
{
// unused
}

void clear_screen()
{
	memset(back_buffer->pixels, 0, pixelbuf_size);
}

void set_color(int color) {
	current_color = color;
}

// write a logical pixel onto the backbuffer, taking as input a physical pixel location
const void put_pixel(point pixel)
{
	// calculate the boundaries of the logical pixel
    	point startpos =
		{ pixel.x * pixels_per_point, pixel.y * pixels_per_point };
	point endpos =
		{ startpos.x + pixels_per_point, startpos.y + pixels_per_point };

	//    if(do_draw_grid) {
	//	    startpos.x++;
	//	    startpos.y++;
	//	    endpos.x--;
	//	    endpos.y--;
    	//}
	
	putpixelpoint(pixel.x, (WINDOW_HEIGHT-pixel.y), current_color);
}
	
void putpixelpoint(int x, int y, int pixel)
{	
    int bpp = back_buffer->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *) back_buffer->pixels + y * back_buffer->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

// draw an antialiased line using Bresenham's line drawing algorithm
// coupled to Wu's anti-aliasing algorithm
void draw_line_antialiased_wu(point a, point b)
{
	int dx = b.x - a.x;
	int dy = b.y - a.y;
	// pix1 is the bresenham algorithm's pixel,
	// pix2 is the additional anti-aliasing pixel
	if((b.x < a.x) && (abs(dx) >= abs(dy))) {
		a.y++;
		b.y++;
	} else if((b.x < a.x) && (abs(dy) > abs(dx))) {
		a.x++;
		b.x++;
	}
    point pix1 = { a.x, a.y };
    point pix2 = { a.x, a.y };
    int fraction;
    int t;
    int color;

    int xinc = 1, yinc = 1;

    if (dx < 0) {
	dx = -dx;
	xinc = -1;
    }

    if (dy < 0) {
	dy = -dy;
	yinc = -1;
    }

    dx = 2 * dx;
    dy = 2 * dy;


    if (dx >= dy) {
	fraction = dy - (dx >> 1);
	t = a.y;
	pix2.y += yinc;
	
	set_color(MAX_COLOR_VALUE >> 1);
	put_pixel(pix1);
	put_pixel(pix2);

	while (pix1.x != b.x) {

	    if (fraction >= 0) {
		pix1.y += yinc;
		pix2.y += yinc;
		fraction -= dx;
	    }

	    pix1.x += xinc;
	    pix2.x += xinc;

	    color = calc_color(fraction, dx);
	    set_color(color);
	    put_pixel(pix1);
	    color = calc_color_inv(fraction, dx);
	    set_color(color);
	    put_pixel(pix2);
	    fraction += dy;
	}
    } else {
	fraction = dx - (dy >> 1);
	t = a.x;
	pix2.x += xinc;
	set_color(MAX_COLOR_VALUE >> 1);
	put_pixel(pix1);
	put_pixel(pix2);

	while (pix1.y != b.y) {

	    if (fraction >= 0) {
		pix1.x += xinc;
		pix2.x += xinc;
		fraction -= dy;
	    }

	    pix1.y += yinc;
	    pix2.y += yinc;

	    color = calc_color(fraction, dy);
	    set_color(color);
	    put_pixel(pix1);
	    color = calc_color_inv(fraction, dy);
	    set_color(color);
	    put_pixel(pix2);

	    fraction += dx;
	}
    }
}

// calculate what color the pixel should be, based
// on how far it is from the ideal line.  This colour should
// be the inverse of the value calculated in calc_color, so
// that the observed color for the two pixels lies on the line
int calc_color_inv(int a, int b)
{
    int intc = 0;
    float error = (float) (a + b) / b;
    error = 1 - error;
    error *= ((float) MAX_COLOR_VALUE);
    if (error < 0)
	error = -error;

    intc = (int) error;
    if (intc == ((float) MAX_COLOR_VALUE))
	intc = 0;
    else if (intc == 0)
	intc = 1;

    return intc;
}

// calculate what color the pixel should be, based
// on how far it is from the ideal line
int calc_color(int a, int b)
{
    int intc = 0;
    float error = (float) (a + b) / b;
    error *= ((float) MAX_COLOR_VALUE);
    
    if (error < 0)
	error = -error;

    intc = (int) error;
    if (intc == ((float) MAX_COLOR_VALUE))
	intc = 0;
    else if (intc == 0)
	intc = 1;

    return intc;
}


// draw a line using Bresenham's line drawing algorithm
void draw_line_bresenham(point a, point b)
{
    point pix1 = { a.x, a.y };
    int fraction;
    int t;

    int xinc = 1, yinc = 1;
    int dx = b.x - a.x;
    int dy = b.y - a.y;

    if (dx < 0) {
	dx = -dx;
	xinc = -1;
    }

    if (dy < 0) {
	dy = -dy;
	yinc = -1;
    }

    dx = 2 * dx;
    dy = 2 * dy;

    if (dx >= dy) {
	fraction = dy - (dx >> 1);
	t = a.y;

	set_color(COLOR_BLACK);
	put_pixel(pix1);

	while (pix1.x != b.x) {

	    if (fraction >= 0) {
		pix1.y += yinc;
		fraction -= dx;
	    }

	    pix1.x += xinc;

	    put_pixel(pix1);
	    fraction += dy;
	}
    } else {

	fraction = dx - (dy >> 1);
	t = a.x;

	put_pixel(pix1);

	while (pix1.y != b.y) {

	    if (fraction >= 0) {
		pix1.x += xinc;
		fraction -= dy;
	    }

	    pix1.y += yinc;

	    put_pixel(pix1);
	    fraction += dx;
	}
    }
}

vector<triangle> tri_list;

void tri_clear() {
	tri_list.clear();
}

void tri_add(triangle t) {
	t.visible = true;
	tri_list.push_back(t);
}

void tri_remove(triangle t1) {
	triangle * t2;
	int i = 0;

	for(i = 0; i < tri_list.size(); i++) {
		if(tri_compare(t1, tri_list[i])) {
			tri_list[i].visible = true;
		}
	}
}

bool tri_compare(triangle t1, triangle t2)
{
	bool identical = false;
	if (	(t1.v1.x == t2.v1.x) &&
	 	(t1.v1.y == t2.v1.y) &&
		(t1.v1.z == t2.v1.z) &&
		(t1.v2.x == t2.v2.x) &&
		(t1.v2.y == t2.v2.y) &&
		(t1.v2.z == t2.v2.z) &&
		(t1.v3.x == t2.v3.x) &&
		(t1.v3.y == t2.v3.y) &&
		(t1.v3.z == t2.v3.z)	)
		identical = true;
	else
		identical = false;

	return identical;
}

bool isvisible(triangle t)
{
	bool visible = true;
		// see if the triangle is visible
		vertex v1 = t.v1;
		vertex v2 = t.v2;
		vertex v3 = t.v3;
		vertex n1,n2;
		// v2-v1
		n1.x = v2.x-v1.x;
		n1.y = v2.y-v1.y;
		n1.z = v2.z-v1.z;

		// v3-v2
		n2.x = v3.x-v2.x;
		n2.y = v3.y-v2.y;
		n2.z = v3.z-v2.z;

		vertex crossp;
		crossproduct(&crossp, n1,n2);
		
		if(crossp.z > 0) {
			visible = false;
		}
	return visible;
}

void tri_render()
{
	for(unsigned int i = 0; i < tri_list.size(); i++ ) {
		draw_triangle(tri_list[i]);
	}
}

void drawPolygon(point points[3], int color)
{
	points[0].y = WINDOW_HEIGHT - points[0].y;
	points[1].y = WINDOW_HEIGHT - points[1].y;
	points[2].y = WINDOW_HEIGHT - points[2].y;
	Sint16 x[3] = {points[0].x, points[1].x, points[2].x};
	Sint16 y[3] = {points[0].y, points[1].y, points[2].y};
	filledPolygonColor(back_buffer, x, y, 3, color);
}

void scale_and_clip(point p[3], point r[3]) {

	
}
