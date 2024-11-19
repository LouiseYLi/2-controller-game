#include "../include/display.h"
#include "../include/input_processing.h"
#include <SDL2/SDL.h>
#include <errno.h>
#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char c = ' ';
    int  yCoord;
    int  xCoord;
    int  direction_y = 0;
    int  direction_x = 0;

    initscr();
    noecho();
    curs_set(0);

    yCoord = (LINES - 1) / 2;
    xCoord = (COLS - 1) / 2;

    do
    {
        clear();
        if(c == 'w')
        {
            direction_y = -1;
            direction_x = 0;
        }
        if(c == 'a')
        {
            direction_y = 0;
            direction_x = -1;
        }
        if(c == 's')
        {
            direction_y = 1;
            direction_x = 0;
        }
        if(c == 'd')
        {
            direction_y = 0;
            direction_x = 1;
        }
        if(hit_borders(COLS, LINES, xCoord, yCoord, direction_x, direction_y) == 1)
        {
            yCoord += direction_y;
            xCoord += direction_x;
        }
        mvaddch(yCoord, xCoord, '*');
        refresh();
        c = (char)getch();
    } while(c == 'w' || c == 'a' || c == 's' || c == 'd');

    endwin();
    return EXIT_SUCCESS;
}
