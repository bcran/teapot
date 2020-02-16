#ifndef _BEZIER_H_
#define _BEZIER_H_

#include <stdio.h>
#include <stdlib.h>

#include "line_util.h"
#include "file_parser.h"


int draw_bezier_surface(vertex * vertices, patch * patches);
int draw_bezier_surface(vertex * vertices, patch * patches, point pov);

#endif
