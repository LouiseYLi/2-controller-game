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

void process_input(const int *total_cols, const int *total_lines, void *data, int *err);

void get_input(const int *total_cols, const int *total_lines, void *data, int *err);

int hit_borders(const int *total_cols, const int *total_lines, void *data, int direction_x, int direction_y);

#endif    // GAME_INPUT_PROCESSING_H
