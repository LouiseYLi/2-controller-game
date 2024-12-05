#include "../include/input_processing.h"
#include "../include/gui.h"
#include "../include/network_utils.h"
#include <SDL2/SDL.h>
#include <errno.h>
#include <ncurses.h>
#include <stdio.h>

#define sleep_time 750000000

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

void clamp_position(player *p, const game *g)
{
    if (p->x < 2) {
        p->x = 2;
    } else if (p->x >= (uint32_t)g->height) {
        p->x = (uint32_t)g->height - 1;
    }

    if (p->y < 2) {
        p->y = 2;
    } else if (p->y >= (uint32_t)g->width) {
        p->y = (uint32_t)g->width - 1;
    }
}

void process_keyboard_input(const game *g, player *p, int *err)
{
    int c = getch();

    // if (c == ERR) {
    //     return;
    // }

    switch (c) {
        case 'w':
            p->y -= 1;
            break;
        case 'a':
            p->x -= 1;
            break;
        case 'd':
            p->x += 1;
            break;
        case 's':
            p->y += 1;
            break;
        default:
            *err = -1;
            return;
    }

    clamp_position(p, g);
}

void process_controller_input(const game *g, player *p, int *err)
{
    SDL_Event event;
    int       direction;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_CONTROLLERBUTTONDOWN)
        {
            // mvaddch((int)p->y, (int)p->x, ' ');
            direction = event.cbutton.button;
            if(direction == SDL_CONTROLLER_BUTTON_DPAD_UP)
            {
            p->y-=1;
            if(p->y < 2) {
                p->y = 2;
            }
        break;
        }
        if(direction == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
        {
            p->x-=1;
            if(p->x < 2) {
            p->x = 2;
        }
        break;
        }
        if(direction == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
        {
            p->x +=1;
            if(p->x > (uint32_t)g->height - 1) {
            p->x = (uint32_t)g->height - 1;
        }
        break;
        }
        if(direction == SDL_CONTROLLER_BUTTON_DPAD_DOWN)
        {
            p->y +=1;
            if(p->y > (uint32_t)g->width - 1) {
            p->y = (uint32_t)g->width - 1;
        }
        break;
        }
            *err = -1;
        }
    }
}

void process_timer_input(const game *g, player *p, int *err)
{
    struct timespec req;
    struct timespec rem;
    int  direction;
    req.tv_sec                  = 0;
    req.tv_nsec                 = sleep_time;
    nanosleep(&req, &rem);
    // Seed the random number generator
    // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp)
    // srand((unsigned int)time(NULL) * p->x * p->y);
    direction = rand() % 4;

    switch (direction) {
        case 0:
            p->y-=1;
        if(p->y < 2) {
            p->y = 2;
        }
        break;
        case 1:
            p->x-=1;
        if(p->x < 2) {
            p->x = 2;
        }
        break;
        case 2:
            p->x+=1;
        if(p->x > (uint32_t)g->height - 1) {
            p->x = (uint32_t)g->height - 1;
        }
        break;
        case 3:
            p->y+=1;
        if(p->y > (uint32_t)g->width - 1) {
            p->y = (uint32_t)g->width - 1;
        }
        break;
        default:
            *err = -1;
            break;
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
