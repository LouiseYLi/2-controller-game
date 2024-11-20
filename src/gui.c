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

void gui(int *err)
{
    player p = {0, 0, 0};

    initscr();
    noecho();
    curs_set(0);

    if(signal(SIGINT, handle_signal) == SIG_ERR)
    {
        perror("Error setting up signal handler");
        return;
    }

    p.y = (LINES - 1) / 2;
    p.x = (COLS - 1) / 2;
    mvaddch(p.y, p.x, '*');
    // TODO: add "other" peer player dot
    refresh();
    do
    {
        clear();
        get_input(&COLS, &LINES, &p, err);
        // TODO: serialize player struct
        // TODO: write serialized data to socket, asynchronous, must SYNC

        // TODO: retrieve and deserialize any data from socket
        // TODO: update gui on this side
        mvaddch(p.y, p.x, '*');
        refresh();
    } while(terminate == 0);

    endwin();
}
