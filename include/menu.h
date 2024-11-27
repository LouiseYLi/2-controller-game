//
// Created by Reece Melnick on 2024-11-22.
//=

#ifndef NCURSESGAME_MENU_H
#define NCURSESGAME_MENU_H

#include <SDL2/SDL.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>

void *work(void *parm);

void print_menu_options(WINDOW *menuWin, const int *highlight, const char **choices);

void browse_options(const int *choice, int *highlight);

int display_menu(void);

#endif    // NCURSESGAME_MENU_H
