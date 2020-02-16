// CS4 Graphics Practical 2
// Rebecca Cran <s9905155@inf.ed.ac.uk>
//

#include <stdio.h>
#include <string.h> // for memset
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "file_parser.h"
#include "line_util.h"
#include "render.h"
#include "bezier.h"
#include "manip.h"

static const int MAX_LINE_LEN = 128;

int main(int argc, char ** argv) 
{
	SDL_Surface * surface = init_graphics();
	if (surface == 0) {
		perror("failed to initialize graphics\n");
		exit(EXIT_FAILURE);
	}
	
	FILE * teapot_file = fp_open("utah_bez.dat");
	if(teapot_file == 0)
		exit(EXIT_FAILURE);

	// ok, we've opened the data file, now start reading lines

	vertex vertices[306];
	memset(&vertices,0,306*sizeof(vertex));
	patch patches[32];
	memset(&patches, 0, 32*sizeof(patch));

	fp_get_vertices(teapot_file, (vertex*) &vertices, 306);
	fp_get_patches(teapot_file, (patch*) &patches, 32);

	float temp = 0;
	for(int i = 0; i < 306; i++) {
		temp = vertices[i].y;
		vertices[i].y = vertices[i].z;
		vertices[i].z = temp;
	}
	
	point rot = {0, 0, 0};
	draw_bezier_surface(vertices, patches, rot);
	tri_render();
	redraw_screen();
while(1)
{
}
	//while(rot.y < 9000) {
	//	clear_screen();
	//	draw_bezier_surface(vertices,patches, rot);
	//	tri_render();
	//	redraw_screen();
	//	rot.y++;
	//}
	
	//handle_events();
	return EXIT_SUCCESS;
}

FILE * fp_open(char * filename)
{
	FILE * file = fopen(filename, "r");
	return file;
}

void fp_close(FILE * file) 
{
	fclose(file);
}

int fp_get_vertices(FILE * file, vertex * vertices, int max_vertex)
{
	int v_num = 0;
	int rc = 0;
	while(v_num < max_vertex && rc == 0) {
		vertex v;
		rc = fp_get_next_vertex(file, &v);
		if(rc != 0) break;
		vertices[v_num] = v;
		v_num++;
	}
		
	return rc;
}

int fp_get_patches(FILE * file, patch * patches, int max_patch)
{
	int p_num = 0;
	int rc = 0;
	while(p_num < max_patch && rc == 0) {
		patch p;
		rc = fp_get_next_patch(file, &p);
		if(rc != 0) break;
		patches[p_num] = p;
		p_num++;
	}
	
	return rc;
}

int fp_get_next_vertex(FILE * f, vertex * v)
{
	int rc = 0;
	
	if(v == 0) rc = -1;
	
	char line[MAX_LINE_LEN];
	memset(line,0,MAX_LINE_LEN);

	while(line[0] != 'V' && rc == 0) {
		if (fgets(line, MAX_LINE_LEN, f) == 0)
			rc = -1;
	}

	float a,b,c;
	if(rc == 0) {
		// we've found a vertex line
		sscanf(line, "V%*d %f %f %f", &a,&b,&c);
		v->x = (float) a;
		v->y = (float) b;
		v->z = (float) c;
	}

	return rc;
}

int fp_get_next_patch(FILE * f, patch * p)
{
	int rc = 0;

	if(p == 0) rc = -1;

	char line[MAX_LINE_LEN];
	memset(&line, 0, MAX_LINE_LEN);

	while(line[0] != 'P' && rc == 0) {
		if(fgets(line, MAX_LINE_LEN, f) == 0)
			rc = -1;
	}

	if(rc == 0) {
		// we've found a patch line
		sscanf(line, "P%*d %d %d  %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &p->p[0], &p->p[1], &p->p[2], &p->p[3], &p->p[4], &p->p[5], &p->p[6], &p->p[7], &p->p[8], &p->p[9], &p->p[10], &p->p[11], &p->p[12], &p->p[13], &p->p[14], &p->p[15]);
	}

	for(int i = 0; i < 16; i++ ) {
		p->p[i]--;
	}
	return rc;
}
