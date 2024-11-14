// #include "../include/display.h"
#include <ncurses.h>
#include <stdlib.h>

int main(void)
{
    initscr();

    char c;
    int x = (LINES-1)/2;
    int y = (COLS-1)/2;
    int direction_x = 0;
    int direction_y = 0;

    do {
        clear();
        if (c == 'w') {
            direction_x = -1;
            direction_y = 0;
        }
        if (c == 'a') {
            direction_x = 0;
            direction_y = -1;
        }
        if (c == 's') {
            direction_x = 1;
            direction_y = 0;
        }
        if (c == 'd') {
            direction_x = 0;
            direction_y = 1;
        }
        if ( (x+direction_x) <= COLS || (y+direction_y) >= LINES || (x+direction_x) == 0 || (y+direction_y) == 0) {
            x += direction_x;
            y += direction_y;
        }
        mvaddch(x, y, '*');
        refresh();
        c=getch();
    } while (c == 'w' || c == 'a' || c == 's' || c == 'd');
    // mvaddstr((LINES-1)/2, (COLS-1-1)/2, "*");
    // mvaddch(LINES - 1, COLS-1, x);

    // } while(x != 2190 && x != 65);

    endwin();
    return EXIT_SUCCESS;
}
