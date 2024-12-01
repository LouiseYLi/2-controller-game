#ifndef GAME_GUI_H
#define GAME_GUI_H

#include "menu.h"
#include <SDL2/SDL.h>
#include <stdint.h>

typedef struct
{
    // cppcheck-suppress unusedStructMember
    int input_type;
    // cppcheck-suppress unusedStructMember
    int width;
    // cppcheck-suppress unusedStructMember
    int height;
    // cppcheck-suppress unusedStructMember
    SDL_GameController *controller;
} game;

typedef struct
{
    // cppcheck-suppress unusedStructMember
    uint32_t id;
    // cppcheck-suppress unusedStructMember
    uint32_t x;
    // cppcheck-suppress unusedStructMember
    uint32_t y;
} player;

void initialize_gui(game *g, const player *p, const player *p2);

#endif    // GAME_GUI_H
