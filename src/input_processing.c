#include "../include/input_processing.h"
#include "../include/gui.h"
#include <SDL2/SDL.h>
#include <errno.h>
#include <stdio.h>

void process_input(void *data, int *err)
{
    input         *player = (input *)data;
    const uint32_t UP     = 11;
    const uint32_t DOWN   = 12;
    const uint32_t LEFT   = 13;
    const uint32_t RIGHT  = 14;

    printf("3");
    if(player->direction == UP)
    {
        player->x += 0;
        player->y += -1;
    }
    else if(player->direction == DOWN)
    {
        player->x += 0;
        player->y += 1;
    }
    else if(player->direction == LEFT)
    {
        player->x += -1;
        player->y += 0;
    }
    else if(player->direction == RIGHT)
    {
        player->x += 1;
        player->y += 0;
    }
    else
    {
        *err = -1;
    }
}

void gather_input(void *data, int *err)
{
    SDL_Event           event;
    input              *button_input = (input *)data;
    SDL_GameController *controller   = NULL;

    if(SDL_Init(SDL_INIT_GAMECONTROLLER) != 0)
    {
        perror("SDL_Init Error");
        *err = errno;
        return;
    }
    printf("2");
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
    while(1)
    {
        while(SDL_PollEvent(&event))
        {
            printf("in here");
            if(event.type == SDL_QUIT)
            {
                printf("quit");
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
                button_input->direction = event.cbutton.button;
                process_input(button_input, err);
                if(err != 0)
                {
                    perror("Error processing the button input.");
                }
                goto done;
            }
            // Not using joystick
            // if(event.type == SDL_CONTROLLERAXISMOTION)
            // {
            //     printf("Axis event: axis %dposition %d\n", event.caxis.axis, event.caxis.value);
            // }
        }
    }
done:
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
