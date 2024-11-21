#include "../include/gui.h"
#include "../include/input_processing.h"
#include <ncurses.h>
#include <signal.h>

void handle_signal(int signal)
{
    if(signal == SIGINT)
    {
        terminate = 1;
    }
}

void gui(/*TODO: add this back void *data,*/ int *err)
{
    player p = {0, 0, 0};

    // TODO: remove lines to disable unused var warning
    // struct network_socket *socket_data = (struct network_socket *)data;

    initscr();
    noecho();
    curs_set(0);

    if(signal(SIGINT, handle_signal) == SIG_ERR)
    {
        perror("Error setting up signal handler");
        return;
    }

    p.y = (uint32_t)(LINES - 1) / 2;
    p.x = (uint32_t)(COLS - 1) / 2;
    mvaddch((int)p.y, (int)p.x, '*');
    // TODO: add "other" peer player dot
    refresh();
    do
    {
        clear();
        get_input(&p, err);
        // TODO: serialize player struct
        // TODO: write serialized data to socket, asynchronous, must SYNC

        // TODO: retrieve and deserialize any data from socket
        // TODO: update gui on this side
        process_input(&COLS, &LINES, &p, err);

        if(*err != 0)
        {
            perror("Error processing the button input.");
        }
        mvaddch((int)p.y, (int)p.x, '*');
        refresh();
    } while(terminate == 0);

    endwin();
}
