#ifndef _FILE_PARSER_H_
#define _FILE_PARSER_H_

#include <stdlib.h>
#include <stdio.h>

struct vertex_v
{
	float x;
	float y;
	float z;
	float w;
};

typedef vertex_v vertex;

struct patch_v
{
	int p[16];
};

typedef patch_v patch;

FILE * fp_open(char * filename);
void fp_close(FILE * file);
int fp_get_vertices(FILE * file, vertex * vertices, int max_vertex);
int fp_get_patches(FILE * file, patch * patches, int max_vertex);
int fp_get_next_vertex(FILE *f, vertex * v);
int  fp_get_next_patch(FILE *f, patch * p);

#endif
