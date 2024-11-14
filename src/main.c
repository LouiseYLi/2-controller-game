// #include "../include/display.h"
#include <ncurses.h>
#include <stdlib.h>

int hit_borders(int xCoord, int yCoord, int direction_x, int direction_y) {
    if ((xCoord + direction_x) >= COLS || (xCoord + direction_x) <= 0) return 0;
    if ((yCoord + direction_y) >= LINES || (yCoord + direction_y) <= -1) return 0;
    return 1;
}

int main(void)
{
    initscr();
    noecho();
    curs_set(0);

    char c;
    int yCoord = (LINES-1)/2;
    int xCoord = (COLS-1)/2;
    int direction_y = 0;
    int direction_x = 0;

    do {
        clear();
        if (c == 'w') {
            direction_y = -1;
            direction_x = 0;
        }
        if (c == 'a') {
            direction_y = 0;
            direction_x = -1;
        }
        if (c == 's') {
            direction_y = 1;
            direction_x = 0;
        }
        if (c == 'd') {
            direction_y = 0;
            direction_x = 1;
        }
        if (hit_borders(xCoord, yCoord, direction_x, direction_y) == 1) {
            yCoord += direction_y;
            xCoord += direction_x;
        }
        mvaddch(yCoord, xCoord, '*');
        refresh();
        c=getch();
    } while (c == 'w' || c == 'a' || c == 's' || c == 'd');
    // mvaddstr((LINES-1)/2, (COLS-1-1)/2, "*");
    // mvaddch(LINES - 1, COLS-1, x);

    // } while(x != 2190 && x != 65);

    endwin();
    return EXIT_SUCCESS;
}
