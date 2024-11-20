#ifndef GAME_GUI_H
#define GAME_GUI_H

#include <stdint.h>

// Here I ignored the warning for terminate because I wanted
//  terminate to act as a global flag for handling SIGINT.
//  I couldn't think of an alternative to not using a
//  non-constant global flag that also avoids compiler
//  warnings.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static int terminate = 0;
#pragma GCC diagnostic pop

void handle_signal(int signal);

void gui(int *err);

#endif    // GAME_GUI_H
