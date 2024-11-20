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
    refresh();
    do
    {
        clear();
        // TODO: invoke functions that retrieves processed input for movement
        // if(hit_borders(COLS, LINES, p.x, p.y))
        get_input(&COLS, &LINES, &p, err);
        mvaddch(p.y, p.x, '*');
        refresh();
        // c = (char)getch();
    } while(terminate == 0);

    endwin();
}
