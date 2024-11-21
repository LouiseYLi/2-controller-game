#ifndef GAME_INPUT_PROCESSING_H
#define GAME_INPUT_PROCESSING_H

#include "network_utils.h"
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>

typedef struct
{
    uint32_t x;
    uint32_t y;
    uint16_t direction;
} player;

void new_player_buffer(uint8_t **buffer, int *err);

void serialize_coordinate(uint32_t coordinate, uint8_t buffer[], long unsigned int *index);

void serialize_direction(uint16_t direction, uint8_t buffer[], long unsigned int *index);

void serialize_player(const player *p, uint8_t buffer[]);

void process_input(const int *total_cols, const int *total_lines, void *data, int *err);

void get_input(void *data, int *err);

int hit_borders(const int *total_cols, const int *total_lines, void *data, int direction_x, int direction_y);

#endif    // GAME_INPUT_PROCESSING_H
