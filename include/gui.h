#ifndef GAME_GUI_H
#define GAME_GUI_H

#include <SDL2/SDL.h>
#include <stdint.h>

typedef struct
{
    int                 input_type;
    int                 width;
    int                 height;
    SDL_GameController *controller;
} game;

typedef struct
{
    uint32_t id;
    uint32_t x;
    uint32_t y;
} player;

void initialize_gui(const game *g, const player *p, const player *p2);

#endif    // GAME_GUI_H
