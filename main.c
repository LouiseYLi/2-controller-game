#include <stdio.h>
#include <ncurses.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int get_controller_input();

int controller_move(SDL_GameController* controller, int box_width, int box_height, int32_t *player_x, int32_t *player_y);
int key_board_move(SDL_GameController* controller, int box_width, int box_height, int32_t * player_x, int32_t * player_y);
int timer_move(int box_width, int box_height, int32_t *player_x, int32_t *player_y, int32_t *prev_x, int32_t *prev_y);

int player_move(int (*fun_ptr)(SDL_GameController* controller, int box_width, int box_height, int32_t * player_x, int32_t * player_y), SDL_GameController* controller, int box_width, int box_height, int32_t * player_x, int32_t * player_y) {
    fun_ptr(controller, box_width, box_height, player_x, player_y);

    return 0;
}

struct move_struct {
   char player_icon;
   int32_t x_pos;
   int32_t y_pos;
};

int handle_server(struct move_struct *player, struct move_struct *player2, const char* myIP, const char* friendIP, int input_choice);

void handle_type_of_input(int (**move_function)(SDL_GameController* controller, int box_width, int box_height, int32_t *x_pos, int32_t *y_pos));


void initialize_screen(struct move_struct player, struct move_struct player2);

#define BUFFER_SIZE 1024
#define PORT 8080
#define BOX_WIDTH 60
#define BOX_HEIGHT 20

// 192.168.0.89

int main(int argc, char *argv[])
{

    int input_choice = atoi(argv[1]);
    char *myIP = argv[2];
    char *friendIP = argv[3];

    struct move_struct player1;
    struct move_struct player2;

    if (input_choice == 1) {
        player1.player_icon = '*';
        player1.x_pos = 5;
        player1.y_pos = 5;

        player2.player_icon = '*';
        player2.x_pos = 10;
        player2.y_pos = 10;
    } else {
        player1.player_icon = '*';
        player1.x_pos = 10;
        player1.y_pos = 10;

        player2.player_icon = '*';
        player2.x_pos = 5;
        player2.y_pos = 5;
    }

  initialize_screen(player1, player2);
  handle_server(&player1, &player2, myIP, friendIP, input_choice);

   return 0;
}

void initialize_screen(struct move_struct player, struct move_struct player2) {

    initscr();
    cbreak();
    noecho();
    curs_set(0);

   WINDOW * win = newwin(BOX_HEIGHT, BOX_WIDTH, 1, 1);

   keypad(win, TRUE);
   refresh();
   box(win , 0, 0);
   wrefresh(win);
   mvaddch(player.y_pos, player.x_pos, player.player_icon);

   mvaddch(player2.y_pos, player2.x_pos, player2.player_icon);

    refresh();
}

int handle_server(struct move_struct *player, struct move_struct *player2, const char* myIP, const char* friendIP, int input_choice) {

    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    mvprintw(30, 50, "SERVER");
    refresh();

    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        mvprintw(1, 45,"Socket fail\n");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(myIP);

    memset(&client_addr, 0, sizeof(server_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(PORT);
    client_addr.sin_addr.s_addr = inet_addr(friendIP);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        close(server_fd);
        return -1;
    }

    ssize_t num_bytes = 0;

    int flags = fcntl(server_fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl get");
        close(server_fd);
    }

    if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl set");
        close(server_fd);
    }

	refresh();

    int (*move_function)(SDL_GameController* controller, int box_width, int box_height, int32_t *x_pos, int32_t *y_pos);


    handle_type_of_input(&move_function);

    int32_t arr[3];
	int32_t client_arr[3];

    SDL_GameController* controller = NULL;

    if (SDL_Init(SDL_INIT_GAMECONTROLLER) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    if (SDL_NumJoysticks() > 0) {
        controller = SDL_GameControllerOpen(0);
        if (!controller) {
            printf("Could not open game controller: %s\n", SDL_GetError());
            SDL_Quit();
            return EXIT_FAILURE;
        }
    } else {
        SDL_Quit();
    }

    while(1) {

        num_bytes = recvfrom(server_fd, client_arr, sizeof(client_arr), 0, (struct sockaddr *)&client_addr, &client_len);
        if (num_bytes > 0) {
            mvaddch(player2->y_pos, player2->x_pos, ' ');
            player2->y_pos = client_arr[1], player2->x_pos = client_arr[0];
            mvaddch(client_arr[1], client_arr[0], player2->player_icon);
            refresh();
        }

        if (input_choice == 1) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_CONTROLLERBUTTONDOWN) {
                    mvaddch(player->y_pos, player->x_pos, ' ');
                    player_move(move_function, controller, BOX_WIDTH, BOX_HEIGHT, &player->x_pos, &player->y_pos);
                    mvaddch(player->y_pos, player->x_pos, player->player_icon);
                }
            }
        } else {
            int prev_x = player->x_pos;
            int prev_y = player->y_pos;
            // player_move(move_function, controller, 40, 20, &player->x_pos, &player->y_pos);
            key_board_move(controller, BOX_WIDTH, BOX_HEIGHT, &player->x_pos, &player->y_pos);
            mvaddch(prev_y, prev_x, ' ');
            mvaddch(player->y_pos, player->x_pos, player->player_icon);
            refresh();
        }

       	arr[0] = player->x_pos, arr[1] = player->y_pos, arr[2] = player->player_icon;

        sendto(server_fd, arr, sizeof(arr), 0, (struct sockaddr *)&client_addr, client_len);

        refresh();
	}
}

void handle_type_of_input(int (**move_function)(SDL_GameController* controller, int box_width, int box_height, int32_t *x_pos, int32_t *y_pos)) {
    if (get_controller_input() == 0) {
        mvprintw(26, 5, "Controller input");
        *move_function = &controller_move;
    } else {
        mvprintw(26, 5, "Keyboard input");
        *move_function = &key_board_move;
    }
}

int key_board_move(SDL_GameController* controller, int box_width, int box_height, int32_t * player_x, int32_t * player_y) {

    nodelay(stdscr, TRUE);

    char c = getch();

        if (c != ERR) {
            refresh();
        } else {
            refresh();
        }

    switch (c) {
        case 'w':
            *player_y-=1;
        if(*player_y < 2) {
            *player_y = 2;
        }
        break;
        case 'a':
            *player_x-=1;
        if(*player_x < 2) {
            *player_x = 2;
        }
        break;
        case 'd':
            *player_x+=1;
        if(*player_x > box_width - 1) {
            *player_x = box_width - 1;
        }
        break;
        case 's':
            *player_y+=1;
        if(*player_y > box_height - 1) {
            *player_y = box_height - 1;
        }
        break;
        default:
            break;
    }
    return 0;
}

int controller_move(SDL_GameController* controller,int box_width, int box_height, int32_t * player_x, int32_t * player_y)
{
    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP)) {
        *player_y-=1;
        if(*player_y < 2) {
            *player_y = 2;
        }
        return SDL_CONTROLLER_BUTTON_DPAD_UP;
    } else if(SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) {
        *player_x-=1;
        if(*player_x < 2) {
            *player_x = 2;
        }
        return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    } else if(SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) {
        *player_x+=1;
        if(*player_x > box_width - 1) {
            *player_x = box_width - 1;
        }
        return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
    } else if(SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) {
        *player_y+=1;
        if(*player_y > box_height - 1) {
            *player_y = box_height - 1;
        }
        return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    } else {
        return -1;
    }
}

int timer_move(int box_width, int box_height, int32_t * player_x, int32_t * player_y, int32_t* prev_x, int32_t* prev_y) {

    usleep(200000);

    int min = 1;
    int max = 4;

    int random_number = min + (rand() % (max - min + 1));

    *prev_x = *player_x;
    *prev_y = *player_y;

    switch (random_number) {
        case 1:
            *player_y-=1;
        if(*player_y < 2) {
            *player_y = 2;
        }
        break;
        case 2:
            *player_x-=1;
        if(*player_x < 2) {
            *player_x = 2;
        }
        break;
        case 3:
            *player_x+=1;
        if(*player_x > box_width - 1) {
            *player_x = box_width - 1;
        }
        break;
        case 4:
            *player_y+=1;
        if(*player_y > box_height - 1) {
            *player_y = box_height - 1;
        }
        break;
        default:
            break;
    }

    return 0;

}

int get_controller_input()
{
    // Initialize SDL with the game controller subsystem
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK |
                 SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    int num_joysticks = SDL_NumJoysticks();
    if (num_joysticks == 0) {
        return EXIT_FAILURE;
    }
    mvprintw(24, 5, "Number of input devices found: %d", num_joysticks);


    // Loop through all detected joysticks
    for(int i = 0; i < num_joysticks; i++)
    {
        // Check if the device is a game controller
        if(SDL_IsGameController(i))
        {
            SDL_GameController* controller;
            controller = SDL_GameControllerOpen(i);
            if(controller)
            {
                mvprintw(25, 5, "Found game controller: %s\n", SDL_GameControllerName(controller));
                SDL_GameControllerClose(controller);
            }
            else
            {
                printf("Could not open game controller %d: %s\n", i, SDL_GetError());
            }
        }
        else
        {
            printf("Device %d is not a game controller.\n", i);
        }
    }
    SDL_Quit();
    return EXIT_SUCCESS;
}
