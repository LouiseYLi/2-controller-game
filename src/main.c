#include "../include/display.h"
#include "../include/gui.h"
#include "../include/input_processing.h"
#include "../include/network_utils.h"
#include <SDL2/SDL.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define temp_coord 5
#define temp_coord2 10

// TODO: implement socket operations
// TODO: implement "other" peer
// TODO: Implement sockets for peer

static void parse_arguments(int argc, char *argv[], void *arg, int *err)
{
    int                    option;
    struct network_socket *data = (struct network_socket *)arg;
    while((option = getopt(argc, argv, "s:d:")) != -1)
    {
        switch(option)
        {
            case 's':
                data->src_ip = optarg;
                break;
            case 'd':
                data->dest_ip = optarg;
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
    const int height = 20;
    const int width  = 30;
    game      g      = {1, height, width};
    player    p      = {0, temp_coord, temp_coord, 0};
    player    p2     = {0, temp_coord2, temp_coord2, 0};
    int       err    = 0;
    int       retval = 0;

    const char *PORT = "9999";
    data.src_ip      = NULL;
    data.dest_ip     = NULL;
    data.port        = convert_port(PORT, &err);
    data.socket_fd   = 0;

    if(err != 0)
    {
        goto done;
    }

    parse_arguments(argc, argv, &data, &err);
    if(err != 0)
    {
        retval = EXIT_FAILURE;
        goto done;
    }

    data.socket_fd = setup_network_socket(data.src_ip, data.port, &err);
    initialize_gui(&g, &p, &p2);

    // cleanup:
    if(data.socket_fd != 0)
    {
        close_socket(data.socket_fd);
    }
done:
    return retval;
}
