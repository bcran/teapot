#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "manip.h"

static const float PI = 3.141592l;
static const float DEG30 = PI/6;
static const float DEG45 = PI/4;
static const float DEG90 = PI/2;
static const float DEG180= PI;

	
static float M[4][4] = {
			{ -1, 3,-3, 1},
			{  3,-6, 3, 0},
			{ -3, 3, 0, 0},
			{  1, 0, 0, 0}
					};

float rotx[4][4] = {  	{1,	   0,		0, 0},                                                                    
		 	{0,cos(DEG45), -sin(DEG45), 0},
			{0,sin(DEG45),  cos(DEG45), 0},
			{0 ,	   0,		0, 1 }        };

float roty[4][4] = {	{cos(DEG45),  0,	-sin(DEG45)},
			{0,1,0},
			{sin(DEG45),0,cos(DEG45)} };

float rotz[4][4] = {	{cos(DEG45),	sin(DEG45), 	0},
			{-sin(DEG45), cos(DEG45),0},
			{0,0,1} };
			

void  mult_matrix_4_4(float a[4][4], float b[4][4], float result[4][4])
{	
	for(int cur_row = 0; cur_row < 4; cur_row++) {
		for(int cur_col = 0; cur_col < 4; cur_col++) {
			result[cur_row][cur_col] = 0;
			for(int c = 0; c < 4; c++) {
				result[cur_row][cur_col] += a[cur_row][c] * b[c][cur_col];
			}
		}
	}
}

void M_G_MT_calc(float G[4][4], float result[4][4])
{
	float temp_result[4][4];
	mult_matrix_4_4(M, G, temp_result);
	mult_matrix_4_4(temp_result,M, result);
}

void mult_S_M_G_MT_T(double s, double t, float m_g_mt[4][4], float * result) 
{
	float s_m_g_mt[4];
	s_m_g_mt[0] = ((s*s*s)*m_g_mt[0][0]) + 
		      ((s*s)  *m_g_mt[1][0]) +
		      ((s)    *m_g_mt[2][0]) +
		      m_g_mt[3][0];
	
	s_m_g_mt[1] = ((s*s*s)*m_g_mt[0][1]) +
		      ((s*s)  *m_g_mt[1][1]) +
		      ((s)    *m_g_mt[2][1]) +
		      m_g_mt[3][1];

	s_m_g_mt[2] = ((s*s*s)*m_g_mt[0][2]) +
		      ((s*s)  *m_g_mt[1][2]) +
		      ((s)    *m_g_mt[2][2]) +
		      m_g_mt[3][2];

	s_m_g_mt[3] = ((s*s*s)*m_g_mt[0][3]) +
		      ((s*s)  *m_g_mt[1][3]) +
		      ((s)    *m_g_mt[2][3]) +
		      m_g_mt[3][3];

	*result = (s_m_g_mt[0]*(t*t*t)) + (s_m_g_mt[1]*(t*t)) + (s_m_g_mt[2]*t) + s_m_g_mt[3];
}

void rotate(vertex * p, int x, int y, int z)
{
	float gm[4][1] = { {p->x}, {p->y}, {p->z}, {1}};

	float newx, newy, newz;

	float radrotx = ((float)x/180.0)*PI;
	float radroty = ((float)y/180.0)*PI;
	float radrotz = ((float)z/180.0)*PI;

	rotx[1][1] = cos(radrotx);
	rotx[1][2] = -sin(radrotx);
	rotx[2][1] = sin(radrotx);
	rotx[2][2] = cos(radrotx);

	newx = 	(rotx[0][0]*gm[0][0]) + (rotx[0][1]*gm[1][0]) + (rotx[0][2]*gm[2][0]) + (rotx[0][3]*gm[3][0]);
	newy =  (rotx[1][0]*gm[0][0]) + (rotx[1][1]*gm[1][0]) + (rotx[1][2]*gm[2][0]) + (rotx[1][3]*gm[3][0]);
	newz =  (rotx[2][0]*gm[0][0]) + (rotx[2][1]*gm[1][0]) + (rotx[2][2]*gm[2][0]) + (rotx[2][3]*gm[3][0]);

	gm[0][0] = newx;
	gm[1][0] = newy;
	gm[2][0] = newz;
	gm[3][0] = 1;

	roty[0][0] = cos(radroty);
	roty[0][2] = -sin(radroty);
	roty[2][0] = sin(radroty);
	roty[2][2] = cos(radroty);

	newx = 	(roty[0][0]*gm[0][0]) + (roty[0][1]*gm[1][0]) + (roty[0][2]*gm[2][0]) + (roty[0][3]*gm[3][0]);
	newy =  (roty[1][0]*gm[0][0]) + (roty[1][1]*gm[1][0]) + (roty[1][2]*gm[2][0]) + (roty[1][3]*gm[3][0]);
	newz =  (roty[2][0]*gm[0][0]) + (roty[2][1]*gm[1][0]) + (roty[2][2]*gm[2][0]) + (roty[2][3]*gm[3][0]);

	gm[0][0] = newx;
	gm[1][0] = newy;
	gm[2][0] = newz;
	gm[3][0] = 1;

	rotz[0][0] =  cos(radrotz);
	rotz[0][1] =  sin(radrotz);
	rotz[1][0] = -sin(radrotz);
	rotz[1][1] =  cos(radrotz);

	newx = 	(rotz[0][0]*gm[0][0]) + (rotz[0][1]*gm[1][0]) + (rotz[0][2]*gm[2][0]) + (rotz[0][3]*gm[3][0]);
	newy =  (rotz[1][0]*gm[0][0]) + (rotz[1][1]*gm[1][0]) + (rotz[1][2]*gm[2][0]) + (rotz[1][3]*gm[3][0]);
	newz =  (rotz[2][0]*gm[0][0]) + (rotz[2][1]*gm[1][0]) + (rotz[2][2]*gm[2][0]) + (rotz[2][3]*gm[3][0]);
	
	p->x = newx;
	p->y = newy;
	p->z = newz;
}

void crossproduct(vertex * r, vertex p, vertex q) {
   	r->x = p.y*q.z - p.z*q.y;
   	r->y = p.z*q.x - p.x*q.z,
	r->z = p.x*q.y - p.y*q.x;
}
