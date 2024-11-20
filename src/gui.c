#include "../include/gui.h"
#include "../include/input_processing.h"
#include <ncurses.h>
#include <signal.h>

// Here I ignored the warning for terminate because I wanted
//  terminate to act as a global flag for handling SIGINT.
//  I couldn't think of an alternative to not using a
//  non-constant global flag that also avoids compiler
//  warnings.
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static int terminate = 0;

void handle_signal(int signal);

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
        get_input(&p, err);
        mvaddch(p.y, p.x, '*');
        refresh();
        // c = (char)getch();
    } while(terminate == 0);

    endwin();
}
