#include "../include/gui.h"
#include <ncurses.h>
#include <signal.h>

void initialize_gui(game *g, const player *p, const player *p2)
{
    WINDOW *gui_window;

    g->input_type = display_menu();

    initscr();
    noecho();
    curs_set(0);
    timeout(0);

    gui_window = newwin(g->width, g->height, 1, 1);
    keypad(gui_window, TRUE);
    refresh();
    box(gui_window, 0, 0);
    wrefresh(gui_window);

    mvaddch((int)p->y, (int)p->x, '*');
    mvaddch((int)p2->y, (int)p2->x, '*');

    // TODO: add "other" peer player dot
    refresh();
}
