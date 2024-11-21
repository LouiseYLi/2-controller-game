#ifndef GAME_INPUT_PROCESSING_H
#define GAME_INPUT_PROCESSING_H

#include "network_utils.h"
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>

typedef struct
{
    int32_t  x;
    int32_t  y;
    uint16_t direction;
} player;

void serialize_coordinate(int32_t coordinate, uint8_t buffer[], int *index);

void serialize_direction(uint16_t direction, uint8_t buffer[], int *index);

void serialize_player(struct player *player, uint8_t buffer[]);

void process_input(const int *total_cols, const int *total_lines, void *data, int *err);

void get_input(void *data, int *err);

int hit_borders(const int *total_cols, const int *total_lines, void *data, int direction_x, int direction_y);

#endif    // GAME_INPUT_PROCESSING_H
