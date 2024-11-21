#ifndef GAME_GUI_H
#define GAME_GUI_H

#include "input_processing.h"
#include <stdint.h>

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

typedef struct
{
    int width;
    int height;
} window;

void handle_signal(int signal);

void initialize_gui(const window *w, const player *p, const player *p2);

#endif    // GAME_GUI_H
