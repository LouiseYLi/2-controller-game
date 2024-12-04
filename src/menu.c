#include "../include/menu.h"
#include <ncurses.h>

void print_menu_options(WINDOW *menuWin, const int *highlight, const char **choices)
{
    const int MESSAGE_POS  = 5;
    const int MESSAGE2_POS = 21;
    mvprintw(3, MESSAGE2_POS, "Welcome to the game!");
    mvprintw(MESSAGE_POS, 2, "Select your input:");
    refresh();
    for(int i = 0; i < 3; i++)
    {
        if(i == *highlight)
        {
            wattron(menuWin, A_REVERSE);
        }
        mvwprintw(menuWin, i + MESSAGE_POS, 1, "%s", choices[i]);
        wattroff(menuWin, A_REVERSE);
    }
}

void browse_options(const int *choice, int *highlight)
{
    switch(*choice)
    {
        case KEY_UP:
            *highlight -= 1;
            break;
        case KEY_DOWN:
            *highlight += 1;
            break;
        default:
            break;
    }
    if(*highlight == 3)
    {
        *highlight = 0;
    }
    if(*highlight == -1)
    {
        *highlight = 2;
    }
}

int display_menu(void)
{
    const int   WINDOW_HEIGHT = 20;
    const int   WINDOW_WIDTH  = 60;
    const char *choices[3]    = {"Keyboard", "Controller", "Timer"};
    int         choice        = -1;
    int         highlight     = 0;
    WINDOW     *menuWin;

    init_pair(4, COLOR_RED, COLOR_MAGENTA);
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    start_color();
    refresh();

    menuWin = newwin(WINDOW_HEIGHT, WINDOW_WIDTH, 1, 1);
    box(menuWin, 0, 0);
    refresh();
    wrefresh(menuWin);
    keypad(menuWin, TRUE);
    //    pthread_t t1;
    //    pthread_create(&t1, NULL, work, NULL);
    while(1)
    {
        if(choice == KEY_RIGHT)
        {
            break;
        }
        print_menu_options(menuWin, &highlight, choices);
        choice = wgetch(menuWin);
        browse_options(&choice, &highlight);
        refresh();
    }
    wborder(menuWin, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(menuWin);
    delwin(menuWin);
    return (highlight + 1);
}
