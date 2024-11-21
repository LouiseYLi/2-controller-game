#include "../include/display.h"
#include "../include/gui.h"
#include "../include/input_processing.h"
#include "../include/network_utils.h"
#include <SDL2/SDL.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: implement socket operations
// TODO: implement "other" peer
// TODO: Implement sockets for peer

static void parse_arguments(int argc, char *argv[], void *arg, int *err)
{
    int                    option;
    struct network_socket *data = (struct network_socket *)arg;
    data->ip                    = NULL;
    while((option = getopt(argc, argv, "i:")) != -1)
    {
        if(option == 'i')
        {
            data->ip = optarg;
        }
        else
        {
            perror("Error: invalid options.");
            *err = -1;
            return;
        }
    }
    if(data->ip == NULL)
    {
        perror("Error: unable to parse ip.");
        *err = -1;
    }
}

int main(int argc, char *argv[])
{
    struct network_socket data;
    int                   err    = 0;
    int                   retval = 0;

    const char *PORT = "9999";
    data.ip          = NULL;
    data.port        = convert_port(PORT, &err);
    data.socket_fd   = 0;

    if(err != 0)
    {
        goto done;
    }

    parse_arguments(argc, argv, &data, &err);
    if(err != 0)
    {
        perror("Error parsing arguments.");
        retval = EXIT_FAILURE;
        goto done;
    }

    printf("ip: %s", data.ip);

    // gui(&err);

    // cleanup:
    if(data.socket_fd != 0)
    {
        close_socket(data.socket_fd);
    }
done:
    return retval;
}
