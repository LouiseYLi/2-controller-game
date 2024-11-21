#include "../include/input_processing.h"
#include "../include/gui.h"
#include <SDL2/SDL.h>
#include <errno.h>
#include <stdio.h>

void serialize_coordinate(uint32_t coordinate, uint8_t buffer[], long unsigned int *index)
{
    coordinate = htonl(coordinate);
    // TODO: realloc size of buffer if not enough space
    //      i think it should be fine as is though
    memcpy(&buffer[*index], &coordinate, sizeof(coordinate));
    *index += sizeof(coordinate);
}

void serialize_direction(uint16_t direction, uint8_t buffer[], long unsigned int *index)
{
    direction = htons(direction);
    memcpy(&buffer[*index], &direction, sizeof(direction));
    *index += sizeof(direction);
}

void serialize_player(const player *p, uint8_t buffer[])
{
    long unsigned int index = 0;
    serialize_coordinate(p->x, buffer, &index);
    serialize_coordinate(p->y, buffer, &index);
    serialize_direction(p->direction, buffer, &index);
}

void process_input(const int *total_cols, const int *total_lines, void *data, int *err)
{
    player        *p           = (player *)data;
    const uint32_t UP          = 11;
    const uint32_t DOWN        = 12;
    const uint32_t LEFT        = 13;
    const uint32_t RIGHT       = 14;
    int            direction_x = 0;
    int            direction_y = 0;

    if(p->direction == UP)
    {
        direction_x = 0;
        direction_y = -1;
    }
    else if(p->direction == DOWN)
    {
        direction_x = 0;
        direction_y = 1;
    }
    else if(p->direction == LEFT)
    {
        direction_x = -1;
        direction_y = 0;
    }
    else if(p->direction == RIGHT)
    {
        direction_x = 1;
        direction_y = 0;
    }
    else
    {
        *err = -1;
    }
    if(!hit_borders(total_cols, total_lines, p, direction_x, direction_y))
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
        p->x += direction_x;
        p->y += direction_y;
#pragma GCC diagnostic pop
    }
}

void get_input(void *data, int *err)
{
    SDL_Event           event;
    player             *p          = (player *)data;
    SDL_GameController *controller = NULL;

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
    while(1)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                goto done;
            }
            if(event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONUP)
            {
                // printf("Button event: button %d%s\n", event.cbutton.button, event.type == SDL_CONTROLLERBUTTONDOWN ? "pressed" : "released");
                //  up:11
                //  down:12
                //  left:13
                //  right:14
                p->direction = event.cbutton.button;
                goto done;
            }
        }
    }
done:
    SDL_GameControllerClose(controller);
    SDL_Quit();
}

int hit_borders(const int *total_cols, const int *total_lines, void *data, int direction_x, int direction_y)
{
    const player *p = (player *)data;
    int           x = (int)p->x;
    int           y = (int)p->y;
    if((x + direction_x) >= *total_cols || (x + direction_x) <= 0)
    {
        return 1;
    }
    if((y + direction_y) >= *total_lines || (y + direction_y) <= -1)
    {
        return 1;
    }
    return 0;
}
