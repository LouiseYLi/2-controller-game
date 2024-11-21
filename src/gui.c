#include "../include/gui.h"
// #include "../include/input_processing.h"
#include <ncurses.h>
#include <signal.h>

// void handle_signal(int signal)
// {
//     if(signal == SIGINT)
//     {
//         terminate = 1;
//     }
// }

void initialize_gui(const window *w, const player *p, const player *p2)
{
    WINDOW *gui_window;

    initscr();
    noecho();
    curs_set(0);

    // if(signal(SIGINT, handle_signal) == SIG_ERR)
    // {
    //     perror("Error setting up signal handler");
    //     return;
    // }

    // p.y = (uint32_t)(LINES - 1) / 2;
    // p.x = (uint32_t)(COLS - 1) / 2;
    gui_window = newwin(w->width, w->height, 1, 1);
    keypad(gui_window, TRUE);
    refresh();
    box(gui_window, 0, 0);
    wrefresh(gui_window);

    mvaddch((int)p->y, (int)p->x, '*');
    mvaddch((int)p2->y, (int)p2->x, '*');
    // TODO: add "other" peer player dot
    refresh();
    getch();
    // do
    // {
    //     clear();
    //     get_input(&p, err);
    //     // TODO: serialize player struct
    //     // TODO: write serialized data to socket, asynchronous, must SYNC

    //     // TODO: retrieve and deserialize any data from socket
    //     // TODO: update gui on this side
    //     process_input(&COLS, &LINES, &p, err);

    //     if(*err != 0)
    //     {
    //         perror("Error processing the button input.");
    //     }
    //     mvaddch((int)p.y, (int)p.x, '*');
    //     refresh();
    // } while(terminate == 0);

    // endwin();
}
