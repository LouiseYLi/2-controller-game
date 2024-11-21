#ifndef GAME_INPUT_PROCESSING_H
#define GAME_INPUT_PROCESSING_H

#include "network_utils.h"
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>

// TODO: add sequence number/packet id to player struct for serializing
//      receiving peer must parse and check packet id
typedef struct
{
    uint32_t id;
    uint32_t x;
    uint32_t y;
    uint16_t direction;
} player;

uint8_t *new_player_buffer(const player *p, int *err);

void serialize_uint32_t(uint32_t value, uint8_t buffer[], long unsigned int *index);

void serialize_direction(uint16_t direction, uint8_t buffer[], long unsigned int *index);

void serialize_player(const player *p, uint8_t buffer[]);

void process_input(const int *total_cols, const int *total_lines, void *data, int *err);

void get_input(void *data, int *err);

int hit_borders(const int *total_cols, const int *total_lines, void *data, int direction_x, int direction_y);

#endif    // GAME_INPUT_PROCESSING_H
