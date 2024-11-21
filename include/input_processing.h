#ifndef GAME_INPUT_PROCESSING_H
#define GAME_INPUT_PROCESSING_H

#include "gui.h"
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>

// TODO: add sequence number/packet id to player struct for serializing
//      receiving peer must parse and check packet id

uint8_t *new_player_buffer(const player *p, int *err);

void serialize_uint32_t(uint32_t value, uint8_t buffer[], long unsigned int *index);

void serialize_direction(uint16_t direction, uint8_t buffer[], long unsigned int *index);

void serialize_player(const player *p, uint8_t buffer[]);

void process_keyboard_input(const window *w, player *p, int *err);

void process_controller_input(const window *w, player *p, int *err);

void get_input(void *data, int *err);

int hit_borders(const window *w, void *data, int direction_x, int direction_y);

#endif    // GAME_INPUT_PROCESSING_H
