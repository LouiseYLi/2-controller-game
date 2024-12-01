#include "../include/gui.h"
#include "../include/input_processing.h"
#include "../include/network_utils.h"
#include <SDL2/SDL.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define P1_INITIAL_X 15
#define P1_INITIAL_Y 8
#define P2_INITIAL_X 35
#define P2_INITIAL_Y 17

// #define sleep_time 100000000

// TODO: implement socket operations
// TODO: implement "other" peer
// TODO: Implement sockets for peer

static void parse_arguments(int argc, char *argv[], void *arg, game *g, int *err)
{
    const int              base = 10;
    char                  *endptr;
    int                    option;
    struct network_socket *data = (struct network_socket *)arg;
    while((option = getopt(argc, argv, "i:s:d:p:")) != -1)
    {
        switch(option)
        {
            case 'i':
                g->input_type = (int)strtol(optarg, &endptr, base);
                if(endptr == optarg)
                {
                    // No conversion was performed
                    perror("Error invalid input type. Input type must be 1 for keyboard, 2 for controller, 3 for timer.");
                }
                else if(*endptr != '\0')
                {
                    // Conversion stopped at a non-digit character
                    perror("Error: Invalid characters found");
                }
                break;
            case 's':
                data->src_ip = optarg;
                break;
            case 'd':
                data->dest_ip = optarg;
                break;
            case 'p':
                data->port = convert_port(optarg, err);
                break;
            default:
                perror("Error: invalid options.");
                *err = -1;
                return;
        }
    }
    if(data->src_ip == NULL || data->dest_ip == NULL)
    {
        perror("Error found. Missing one or more ip addresses.");
        *err = -1;
    }
}

int main(int argc, char *argv[])
{
    struct network_socket data;
    // Temp default values for window and players
    const int MAX_ARGS = 9;
    const int height   = 25;
    const int width    = 50;
    game      g        = {0, height, width, NULL};
    player    p        = {0, P1_INITIAL_X, P1_INITIAL_Y};
    player    p2       = {0, P2_INITIAL_X, P2_INITIAL_Y};
    int       err      = 0;
    int       retval   = 0;

    data.src_ip    = NULL;
    data.dest_ip   = NULL;
    data.socket_fd = 0;

    if(MAX_ARGS != argc)
    {
        perror("Error invalid number of arguments");
        retval = EXIT_FAILURE;
        return retval;
    }

    parse_arguments(argc, argv, &data, &g, &err);
    if(err != 0)
    {
        retval = EXIT_FAILURE;
        goto done;
    }

    // 1 for keyb, 2 for contr, 3 for timer
    if(g.input_type == 2)
    {
        initialize_controller(g.controller, &err);
        if(err != 0)
        {
            retval = EXIT_FAILURE;
            goto done;
        }
    }

    setup_host_socket(&data, &err);
    if(err != 0)
    {
        perror("Error creating socket.");
        retval = EXIT_FAILURE;
        goto done;
    }

    printf("socket fd %d", data.socket_fd);

    initialize_gui(&g, &p, &p2);
    handle_peer(&data, &g, &p, &p2, &err);
    // cppcheck-suppress knownConditionTrueFalse
    if(err != 0)
    {
        perror("Error in handle_peer.");
        retval = EXIT_FAILURE;
        goto done;
    }

    // cleanup:
done:
    if(data.socket_fd != 0 && data.socket_fd != -1)
    {
        close_socket(data.socket_fd);
    }
    return retval;
}
