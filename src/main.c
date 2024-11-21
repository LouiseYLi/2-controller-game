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

int main(void)
{
    struct network_socket data;
    int err = 0;
    int retval = 0;
    
    const char *PORT = "9999";
    data.ip = NULL;
    data.port = convert_port(PORT, &err);
    data.socket_fd = 0;

    gui(&err);

    // cleanup:
    if (data.socket_fd != 0) {
        close_socket(data.socket_fd);
    }
    // done:
    return retval;
}
