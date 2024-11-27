//
// Created by Reece Melnick on 2024-11-22.
//

#include "menu.h"
#include <SDL2/SDL.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>

//
// int main(void) {
//
//    display_menu();
//}

// static int thread_flag = 0;

// void *work(void *parm)
//{
//     init_pair(1, COLOR_BLACK, COLOR_RED);
//     init_pair(2, COLOR_BLACK, COLOR_BLUE);
//     init_pair(3, COLOR_BLACK, COLOR_GREEN);
//     init_pair(4, COLOR_BLACK, COLOR_MAGENTA);
//     init_pair(5, COLOR_BLACK, COLOR_CYAN);
//     init_pair(6, COLOR_BLACK, COLOR_YELLOW);
//     int i = 1;
//     while(thread_flag == 0)
//     {
//         refresh();
//         attron(COLOR_PAIR(i));
//         mvprintw(3, 21, "Welcome to the game!");
//         refresh();
//         attroff(COLOR_PAIR(i));
//         usleep(100000);
//         i++;
//         if(i == 7)
//         {
//             i = 1;
//         }
//     }
//     return NULL;
// }

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
    const char *choices[3]    = {"Controller", "Keyboard", "Timer"};
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
