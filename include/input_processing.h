#include <SDL2/SDL.h>
#include <stdio.h>

void gather_input(int *err);

int hit_borders(int total_cols, int total_lines, int xCoord, int yCoord, int direction_x, int direction_y);
