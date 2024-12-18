#include "../include/input_processing.h"
#include "../include/gui.h"
#include "../include/network_utils.h"
#include <SDL2/SDL.h>
#include <errno.h>
#include <ncurses.h>
#include <stdio.h>

#define sleep_time 250000000

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
    else if(g->input_type == 2)
    {
        *func = &process_controller_input;
    }
    else if(g->input_type == 3)
    {
        *func = &process_timer_input;
    }
}

void process_keyboard_input(const game *g, player *p, int *err)
{
    int direction;
    int direction_x = 0;
    int direction_y = 0;

    nodelay(stdscr, TRUE);
    // TODO: remove direction from struct
    direction = getch();
    refresh();

    if(direction == 'w')
    {
        direction_x = 0;
        direction_y = -1;
    }
    if(direction == 'a')
    {
        direction_x = -1;
        direction_y = 0;
    }
    if(direction == 's')
    {
        direction_x = 0;
        direction_y = 1;
    }
    if(direction == 'd')
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
    SDL_Event event;
    int       direction;
    int       direction_x = 0;
    int       direction_y = 0;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_CONTROLLERBUTTONDOWN)
        {
            mvaddch((int)p->y, (int)p->x, ' ');
            direction = event.cbutton.button;
            if(direction == SDL_CONTROLLER_BUTTON_DPAD_UP)
            {
                direction_x = 0;
                direction_y = -1;
            }
            else if(direction == SDL_CONTROLLER_BUTTON_DPAD_DOWN)
            {
                direction_x = 0;
                direction_y = 1;
            }
            else if(direction == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
            {
                direction_x = -1;
                direction_y = 0;
            }
            else if(direction == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
            {
                direction_x = 1;
                direction_y = 0;
            }
            else
            {
                *err = -1;
            }
        }
    }
    if(!hit_borders(g, p, direction_x, direction_y))
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
        p->x += direction_x;
        p->y += direction_y;
#pragma GCC diagnostic pop
        mvaddch((int)p->y, (int)p->x, '*');
    }
}

void process_timer_input(const game *g, player *p, int *err)
{
    struct timespec req;
    struct timespec rem;
    int             direction;
    int             direction_x = 0;
    int             direction_y = 0;
    req.tv_sec                  = 0;
    req.tv_nsec                 = sleep_time;
    nanosleep(&req, &rem);
    // Seed the random number generator
    // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp)
    srand((unsigned int)time(NULL) * p->x * p->y);
    direction = rand() % 4;
    if(direction == 0)
    {
        direction_x = 0;
        direction_y = -1;
    }
    if(direction == 1)
    {
        direction_x = -1;
        direction_y = 0;
    }
    if(direction == 2)
    {
        direction_x = 0;
        direction_y = 1;
    }
    if(direction == 3)
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
        // can actually move this to hit borders
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
        p->x += direction_x;
        p->y += direction_y;
#pragma GCC diagnostic pop
    }
}

void initialize_controller(const SDL_GameController *controller, int *err)
{
    if(SDL_Init(SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        *err = errno;
        return;
    }
    if(SDL_NumJoysticks() > 0)
    {
        controller = SDL_GameControllerOpen(0);
        if(!controller)
        {
            printf("Could not open game controller: %s\n", SDL_GetError());
            SDL_Quit();
            *err = errno;
            return;
        }
    }
    else
    {
        SDL_Quit();
        *err = errno;
    }
}

int hit_borders(const game *g, void *data, int direction_x, int direction_y)
{
    const player *p = (player *)data;
    int           x = (int)p->x;
    int           y = (int)p->y;
    if((x + direction_x) >= g->height - 1 || (x + direction_x) <= 2)
    {
        return 1;
    }
    if((y + direction_y) >= g->width || (y + direction_y) <= 1)
    {
        return 1;
    }
    return 0;
}
