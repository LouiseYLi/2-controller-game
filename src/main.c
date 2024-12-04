#include "../include/gui.h"
#include "../include/input_processing.h"
#include "../include/menu.h"
#include "../include/network_utils.h"
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

static void parse_arguments(int argc, char *argv[], void *arg, int *err)
{
    int                    option;
    struct network_socket *data = (struct network_socket *)arg;
    while((option = getopt(argc, argv, "i:s:d:p:")) != -1)
    {
        switch(option)
        {
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
    const int             MAX_ARGS = 7;
    const int             height   = 20;
    const int             width    = 60;
    game                  g        = {0, height, width, NULL};
    player                p        = {0, P1_INITIAL_X, P1_INITIAL_Y};
    player                p2       = {0, P2_INITIAL_X, P2_INITIAL_Y};
    int                   err      = 0;
    int                   retval   = 0;
    int setup_socket_res;

    data.src_ip          = NULL;
    data.dest_ip         = NULL;
    data.socket_fd       = 0;
    data.current_seq_num = 0;

    if(MAX_ARGS != argc)
    {
        perror("Error invalid number of arguments");
        retval = EXIT_FAILURE;
        return retval;
    }

    parse_arguments(argc, argv, &data, &err);
    if(err != 0)
    {
        retval = EXIT_FAILURE;
        goto done;
    }

    setup_socket_res = setup_host_socket(&data, &err);
    if(setup_socket_res == -1)
    {
        perror("Error setting up socket.");
        retval = EXIT_FAILURE;
        goto done;
    }
    
    if(err != 0)
    {
        perror("Error creating socket.");
        retval = EXIT_FAILURE;
        goto done;
    }

    initialize_gui(&g, &p, &p2);

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

    handle_peer(&data, &g, &p, &p2, &err);
    if(err != 0 && errno != EINTR)
    {
        retval = EXIT_FAILURE;
        goto done;
    }

done:
    close(data.socket_fd);
    return retval;
}
