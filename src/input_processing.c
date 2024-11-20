#include "../include/input_processing.h"
#include <SDL2/SDL.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>

// Here I ignored the warning for terminate because I wanted
//  terminate to act as a global flag for handling SIGINT.
//  I couldn't think of an alternative to not using a
//  non-constant global flag that also avoids compiler
//  warnings.
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static int terminate = 0;

void handle_signal(int signal);

void handle_signal(int signal)
{
    if(signal == SIGINT)
    {
        terminate = 1;
    }
}

void gather_input(int *err)
{
    SDL_Event           event;
    SDL_GameController *controller = NULL;

    if(signal(SIGINT, handle_signal) == SIG_ERR)
    {
        perror("Error setting up signal handler");
        return;
    }
    if(SDL_Init(SDL_INIT_GAMECONTROLLER) != 0)
    {
        perror("SDL_Init Error");
        *err = errno;
        return;
    }
    if(SDL_NumJoysticks() > 0)
    {
        controller = SDL_GameControllerOpen(0);
        if(!controller)
        {
            perror("Unable to open game controller");
            *err = errno;
            SDL_Quit();
            return;
        }
    }
    else
    {
        perror("No game controllers connected.\n");
        *err = errno;
        SDL_Quit();
        return;
    }
    while(terminate == 0)
    {
        while(SDL_PollEvent(&event))
        {
            printf("in here.");
            if(event.type == SDL_QUIT)
            {
                SDL_GameControllerClose(controller);
                SDL_Quit();
                return;
            }
            if(event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONUP)
            {
                printf("Button event: button %d%s\n", event.cbutton.button, event.type == SDL_CONTROLLERBUTTONDOWN ? "pressed" : "released");
                // TODO: process input... event.cbutton.button
                //  up:11
                //  down:12
                //  left:13
                //  right:14
            }
            // Not using joystick
            // if(event.type == SDL_CONTROLLERAXISMOTION)
            // {
            //     printf("Axis event: axis %dposition %d\n", event.caxis.axis, event.caxis.value);
            // }
        }
    }
    SDL_GameControllerClose(controller);
    SDL_Quit();
}

int hit_borders(int total_cols, int total_lines, int xCoord, int yCoord, int direction_x, int direction_y)
{
    if((xCoord + direction_x) >= total_cols || (xCoord + direction_x) <= 0)
    {
        return 0;
    }
    if((yCoord + direction_y) >= total_lines || (yCoord + direction_y) <= -1)
    {
        return 0;
    }
    return 1;
}
