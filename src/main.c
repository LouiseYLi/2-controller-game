#include "../include/display.h"
#include "../include/gui.h"
#include "../include/input_processing.h"
#include <SDL2/SDL.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int err = 0;
    gui(&err);
}
