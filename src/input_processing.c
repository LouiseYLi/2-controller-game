#include "../include/input_processing.h"
#include "../include/gui.h"
#include "../include/network_utils.h"
#include <SDL2/SDL.h>
#include <errno.h>
#include <ncurses.h>
#include <stdio.h>

uint8_t *new_player_buffer(const player *p, int *err)
{
    uint8_t *buffer = (uint8_t *)malloc(sizeof(p->id) + sizeof(p->x) + sizeof(p->y));
    if(buffer == NULL)
    {
        perror("Error allocating memory to player buffer.");
        *err = errno;
        return NULL;
    }
    // Copies player content into buffer
    // memcpy(buffer, p, sizeof(*p));

    return buffer;
}

void serialize_uint32_t(uint32_t value, uint8_t buffer[], long unsigned int *index)
{
    value = htonl(value);
    // TODO: realloc size of buffer if not enough space
    //      i think it should be fine as is though
    memcpy(&buffer[*index], &value, sizeof(value));
    *index += sizeof(value);
}

// TODO: Make sure coordinates are updated before serializing/pickling

void serialize_player(const player *p, uint8_t buffer[])
{
    long unsigned int index = 0;
    serialize_uint32_t(p->id, buffer, &index);
    serialize_uint32_t(p->x, buffer, &index);
    serialize_uint32_t(p->y, buffer, &index);
}

void set_move_function(const game *g, move_function_p *func)
{
    if(g->input_type == 1)
    {
        *func = &process_keyboard_input;
    }
}

void process_keyboard_input(const game *g, player *p, int *err)
{
    int direction_x = 0;
    int direction_y = 0;

    // nodelay(stdscr, TRUE);
    p->direction = getch();

    if(p->direction == 'w')
    {
        direction_x = -1;
        direction_y = 0;
    }
    if(p->direction == 'a')
    {
        direction_x = 0;
        direction_y = -1;
    }
    if(p->direction == 's')
    {
        direction_x = 1;
        direction_y = 0;
    }
    if(p->direction == 'd')
    {
        direction_x = 0;
        direction_y = 1;
    }
    else
    {
        *err = -1;
    }
    if(!hit_borders(g, p, direction_x, direction_y))
    {
        // can actually move this to hit borders
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
        p->x += direction_x;
        p->y += direction_y;
#pragma GCC diagnostic pop
    }
}

void process_controller_input(const game *g, player *p, int *err)
{
    const int UP          = 11;
    const int DOWN        = 12;
    const int LEFT        = 13;
    const int RIGHT       = 14;
    int       direction_x = 0;
    int       direction_y = 0;

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
    if(!hit_borders(g, p, direction_x, direction_y))
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

int hit_borders(const game *g, void *data, int direction_x, int direction_y)
{
    const player *p = (player *)data;
    int           x = (int)p->x;
    int           y = (int)p->y;
    if((x + direction_x) >= g->height || (x + direction_x) <= 0)
    {
        return 1;
    }
    if((y + direction_y) >= g->width || (y + direction_y) <= -1)
    {
        return 1;
    }
    return 0;
}
