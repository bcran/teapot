#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

#include "bezier.h"
#include "line_util.h"
#include "file_parser.h"
#include "render.h"
#include "manip.h"

int draw_bezier_surface(vertex * vertices, patch * patches) 
{
	point pov = {45, 0, 0};
	return draw_bezier_surface(vertices, patches, pov);
}

int draw_bezier_surface(vertex * vertices, patch * patches, point rotation)
{
	float x_c[4][4], y_c[4][4], z_c[4][4];
	float    x_1, y_1, z_1;
	double    s,t;
	
	const int INTERPOLATION_POINTS = 15;
		
	float m_g_mt__x[4][4];
	float m_g_mt__y[4][4];
	float m_g_mt__z[4][4];

	tri_clear();
	vertex vert[INTERPOLATION_POINTS+1][INTERPOLATION_POINTS+1];
	
	for (int patch = 0; patch < 32; patch++) {

		int arraypos;
		memset(vert,0,sizeof(vertex) * ((INTERPOLATION_POINTS+1)*(INTERPOLATION_POINTS+1)));
		
		for (int row = 0; row < 4; row++) {
			for(int col = 0; col < 4; col++) {
				arraypos = (4*row) + col;
				x_c[row][col] = vertices[patches[patch].p[arraypos]].x;
				y_c[row][col] = vertices[patches[patch].p[arraypos]].y;
				z_c[row][col] = vertices[patches[patch].p[arraypos]].z;
			}
		}
		
		M_G_MT_calc(x_c, m_g_mt__x);
		M_G_MT_calc(y_c, m_g_mt__y);
		M_G_MT_calc(z_c, m_g_mt__z);

		//float t_ip, s_ip;
		double t_inc = (double) 1.0/INTERPOLATION_POINTS;
		double s_inc = (double) 1.0/INTERPOLATION_POINTS;
		int row = 0, col = 0;
		
		for(t = 0; t <= 1.0+t_inc; t+= t_inc) {
			row = (int) ((float) t * (INTERPOLATION_POINTS));
			for(s = 0; s <= 1.0+s_inc; s+=s_inc) {
				col = (int) ((float)(s*INTERPOLATION_POINTS));
				
				mult_S_M_G_MT_T(s, t, m_g_mt__x, &x_1);
				mult_S_M_G_MT_T(s, t, m_g_mt__y, &y_1);
				mult_S_M_G_MT_T(s, t, m_g_mt__z, &z_1);
			
				vert[row][col].x = x_1;
				vert[row][col].y = y_1;
				vert[row][col].z = z_1;
				rotate(&vert[row][col], 0, rotation.y, 0);
				rotate(&vert[row][col], rotation.x, 0, 0);
				rotate(&vert[row][col], 0, 0, rotation.z);
			}
		}
		
		// ok, we've got a X*X array
		for(int row = 0; row < INTERPOLATION_POINTS; row++) {
			for(int col = 0; col < INTERPOLATION_POINTS; col++) {
				triangle t1 = {vert[row][col], vert[row+1][col+1], vert[row][col+1]};
				triangle t2 = {vert[row][col], vert[row+1][col],   vert[row+1][col+1]};
				if(isvisible(t1))
					tri_add(t1);
				if(isvisible(t2))
					tri_add(t2);
			}
		}
	}

	return 0;
}

