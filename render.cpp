#include <stdio.h>
#include <string.h> // for memset
#include <stdlib.h>
#include <math.h>

extern "C" {
#include <SDL.h>
}

#include "render.h"
#include "line_util.h"
#include "file_parser.h"
#include "manip.h"


// z buffer
// when writing pixels, need z coord of point being drawn

void draw_3d_line(line_3d l)
{	
	vertex v = {l.a.x, l.a.y, l.a.z};
	vertex u = {l.b.x, l.b.y, l.b.z};

	
	l.a.x = v.x;
	l.a.y = v.y;
	l.a.z = v.z;
	l.b.x = u.x;
	l.b.y = u.y;
	l.b.z = u.z;
	
	point a = {(int) (100*(l.a.x)), (int) (100*l.a.y)};
	point b = {(int) (100*(l.b.x)), (int) (100*l.b.y)};	
	
	a.x += (int) (grid_size/2.0);
	a.y += (int) (grid_size/2.0);
	b.x += (int) (grid_size/2.0);
	b.y += (int) (grid_size/2.0);
/*
	double coeff_x = (b.x - a.x);
	double coeff_y = (b.y - a.y);
	double coeff_z = (b.z - a.z);

	if(v.z < u.z) {
		printf("v is smaller\n");
		char * zbuf = (char*) malloc(500000);
		2x + 3y + z = 10;
		x = 2, y = 1, z = 3;
		x = 4, y = -2, z = 8;

		cox = 2, coy = 3, coz = -5;

		2x + y + 3z = 0;
		4x -2y + 8z = 0;
		
		4x + 2y + 6z = 0;
		4x - 2y + 8z = 0;
		4y + -2z = 0;
		4y = 2z;
		
		-4x -2y -6z = 0;
		 4x -2y +8z = 0;

		 -8x - 14z = 0;
		 -8x = 14z;
		 z = -0.57142857x;

		 2  1 3;
		 4 -2 8;

		 (2*(8/3)) (1*(8/3)) (8);
		 - top, bottom;
		 ax by;
		 a/b;
		 **** y = ax;

		 repeat for y;
		 **** z = ax;

		 increment x, get y and z;
		 
		*/		
	draw_line(a, b);
}

void draw_3d_point(vertex a) 
{
	a.x = (int) ((grid_size/2.0) + (100*a.x));
	a.y = (int) ((grid_size/2.0) + (100*a.y));
	point p = {(int) a.x, (int) a.y};
	put_pixel(p);
}


// take in a triangle consisting of 3 lines, and output a 2D description
// of the shape to draw, complete with distance information
void draw_triangle(triangle t)
{
	//printf("X of v1 is %f\n", t.v1.x);
	//printf("Z of v1 is %f\n", t.v1.z);
	point points[3];
	memset(&points,0,3);

	points[0].x = (int) (100*t.v1.x) + (int) (grid_size/2.0);
	points[0].y = (int) (100*t.v1.y) + (int) (grid_size/2.0);
	points[1].x = (int) (100*t.v2.x) + (int) (grid_size/2.0);
	points[1].y = (int) (100*t.v2.y) + (int) (grid_size/2.0); 
	points[2].x = (int) (100*t.v3.x) + (int) (grid_size/2.0);
	points[2].y = (int) (100*t.v3.y) + (int) (grid_size/2.0);

	static int biggest_x = 0;
	static int biggest_y = 0;
	static int biggest_z = 0;

	if(points[0].x > biggest_x)
		biggest_x = points[0].x;
	if(points[1].x > biggest_x)
		biggest_x = points[1].x;
	if(points[2].x > biggest_x)
		biggest_x = points[2].x;
	if(points[0].y > biggest_y)
		biggest_y = points[0].y;
	if(points[1].y > biggest_y)
		biggest_y = points[1].y;
	if(points[2].y > biggest_y)
		biggest_y = points[2].y;
	if(points[0].z > biggest_z)
		biggest_z = points[0].z;
	if(points[1].z > biggest_z)
		biggest_z = points[1].z;
	if(points[2].z > biggest_z)
		biggest_z = points[2].z;

	//points[0].x *= 4;
	//points[0].y *= 4;
	//points[1].x *= 4;
	//points[1].y *= 4;
	//points[2].x *= 4;
	//points[2].y *= 4;

	int ipntx[3] = {points[0].x, points[1].x, points[2].x};
	int ipnty[3] = {points[0].y, points[1].y, points[2].y};


	// TODO: check polygons vertices aren't too close to each other, or worse, collide
	//printf("(%d,%d), (%d,%d), (%d,%d)\n", points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y);

		line_3d l1 = {t.v1,  t.v2};
		line_3d l2 = {t.v2, t.v3};
		line_3d l3 = {t.v1, t.v3};
	
		// ok, we've got our 3 points to draw the triangle. Now
		// let's fill it.
		//					l1
		// l1 and l2 share v2         v1--------------v2
		// l2 and l3 share v3	    l3	---	    --- l2
		// l1 and l3 share v1              -----v3--
	
		//scale_and_clip(p, rp); 
	
		// which is topmost?
		//if(p1.y < p2.y && p1.y < p3.y) { // then p1 is top
		//	// calculate the intercept of the line 
		//}	
        //printf("(%d,%d), (%d,%d), (%d,%d)\n", points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y);

		double z = t.v1.z;
		if(z < 0)
			z = -z;
		unsigned long rem = (unsigned long) (80.0*(3.0-z));
		unsigned long cf = 0xffffffff - (rem);
		//printf("%f\n", t.v1.z);
	
		drawPolygon(points, cf);
//	}
	//draw_line(points[0], points[1]);
	//draw_line(points[1], points[2]);
	//draw_line(points[2], points[0]);
}

void tri_fill(triangle t)
{
	//t.v1v2v3;
	// we want to fill from top to bottom
	
}
