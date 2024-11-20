#ifndef GAME_INPUT_PROCESSING_H
#define GAME_INPUT_PROCESSING_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>

typedef struct
{
    int      x;
    int      y;
    uint32_t direction;
} player;

void process_input(void *data, int *err);

void gather_input(void *data, int *err);

int hit_borders(int total_cols, int total_lines, int xCoord, int yCoord, int direction_x, int direction_y);

#endif    // GAME_INPUT_PROCESSING_H
