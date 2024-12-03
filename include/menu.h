//
// Created by Reece Melnick on 2024-11-22.
//=
#ifndef NCURSESGAME_MENU_H
#define NCURSESGAME_MENU_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-default"
#ifdef clang
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wreserved-macro-identifier"
    #pragma clang diagnostic ignored "-Wreserved-identifier"
    #pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif
#include <SDL2/SDL.h>
#pragma GCC diagnostic pop
#ifdef clang
    #pragma clang diagnostic pop
#endif

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

void print_menu_options(WINDOW *menuWin, const int *highlight, const char **choices);

void browse_options(const int *choice, int *highlight);

int display_menu(void);

#endif    // NCURSESGAME_MENU_H
