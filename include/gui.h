#ifndef GAME_GUI_H
#define GAME_GUI_H

#include <stdint.h>

typedef struct
{
    int input_type;
    int width;
    int height;
} game;

typedef struct
{
    uint32_t id;
    uint32_t x;
    uint32_t y;
    uint16_t direction;
} player;

// Here I ignored the warning for terminate because I wanted
//  terminate to act as a global flag for handling SIGINT.
//  I couldn't think of an alternative to not using a
//  non-constant global flag that also avoids compiler
//  warnings.
// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wunused-variable"
// // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
// static int terminate = 0;
// #pragma GCC diagnostic pop

void handle_signal(int signal);

void initialize_gui(const game *g, const player *p, const player *p2);

#endif    // GAME_GUI_H
