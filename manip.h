#ifndef _MANIP_H_
#define _MANIP_H_

#include "file_parser.h"
#include "line_util.h"

void mult_matrix(float a[4][4], float b[4][4], float result[4][4]);
void M_G_MT_calc(float G[4][4], float result[4][4]);
void mult_S_M_G_MT_T(double s, double t, float m_g_mt[4][4], float * result);
void rotate(vertex * p, int x, int y, int z);
void crossproduct(vertex *res, vertex p, vertex q);


#endif
