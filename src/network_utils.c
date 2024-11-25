#include "../include/network_utils.h"
#include "../include/gui.h"
#include <ncurses.h>
#include <stdio.h>

#define ERR_NONE 0
#define ERR_NO_DIGITS 1
#define ERR_OUT_OF_RANGE 2
#define ERR_INVALID_CHARS 3

#define TEMPA 35

#define TEMPHUNNO 100

// #define TEMPB 35

void handle_signal(int signal)
{
    if(signal == SIGINT)
    {
        terminate = 1;
    }
}

in_port_t convert_port(const char *str, int *err)
{
    in_port_t port;
    char     *endptr;
    long      val;

    *err  = ERR_NONE;
    port  = 0;
    errno = 0;
    val   = strtol(str, &endptr, 10);    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

    // Check if no digits were found
    if(endptr == str)
    {
        perror("Error no digits found for port.");
        *err = ERR_NO_DIGITS;
        goto done;
    }

    // Check for out-of-range errors
    if(val < 0 || val > UINT16_MAX)
    {
        perror("Error port out of range.");
        *err = ERR_OUT_OF_RANGE;
        goto done;
    }

    // Check for trailing invalid characters
    if(*endptr != '\0')
    {
        perror("Error invalid characters for port.");
        *err = ERR_INVALID_CHARS;
        goto done;
    }

    port = (in_port_t)val;

done:
    return port;
}

struct sockaddr_in *setup_network_address(struct sockaddr_in *addr, socklen_t *addr_len, const char *address, in_port_t port)
{
    in_port_t net_port;

    *addr_len = 0;
    net_port  = htons(port);
    memset(addr, 0, sizeof(*addr));

    *addr_len             = sizeof(*addr);
    addr->sin_family      = AF_INET;
    addr->sin_port        = net_port;
    addr->sin_addr.s_addr = inet_addr(address);
    return addr;

    // fprintf(stderr, "%s is not an IPv4 or an IPv6 address\n", address);
    // *err = errno;
    // return NULL;
}

void set_socket_flags(int socket_fd, int *err)
{
    int flags;
    int result;
    // Returns flags of socket
    flags = fcntl(socket_fd, F_GETFL, 0);

    if(flags == -1)
    {
        close(socket_fd);
        *err = errno;
        return;
    }

    // Sets non-blocking flag to socket
    result = fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
    printf("%d", result);
    if(result == -1)
    {
        close(socket_fd);
        *err = errno;
        return;
    }
}

void bind_network_socket(int socket_fd, const struct sockaddr *addr, socklen_t addr_len, int *err)
{
    int result = bind(socket_fd, addr, addr_len);

    if(result == -1)
    {
        close(socket_fd);
        *err = errno;
        return;
    }
}

int setup_network_socket(struct network_socket *data, const char *local_address, const char *non_local_address, in_port_t port, int *err)
{
    struct sockaddr_in addr;
    socklen_t          addr_len;
    socklen_t          peer_addr_len;
    int                socket_fd;

    // I dereferenced addr to get the address of it
    // Assigned local ipv4 addr to member in struct
    data->src_ipv4_addr = setup_network_address(&addr, &addr_len, local_address, port);
    // if(errno != 0)
    // {
    //     perror("Error in setting up local ipv4 address.");
    //     socket_fd = -1;
    //     goto done;
    // }

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);    // NOLINT(android-cloexec-socket)

    if(socket_fd == -1)
    {
        perror("Error in creating socket.");
        *err = errno;
        goto done;
    }

    set_socket_flags(socket_fd, err);
    if(*err != 0)
    {
        perror("Error setting socket flag to non-blocking.");
        socket_fd = -1;
        goto done;
    }

    bind_network_socket(socket_fd, (struct sockaddr *)data->src_ipv4_addr, addr_len, err);
    if(*err != 0)
    {
        perror("Error binding socket.");
        socket_fd = -1;
        goto done;
    }

    // Set up non-local ipv4 addr and assign to struct
    data->dest_ipv4_addr = setup_network_address(&addr, &peer_addr_len, non_local_address, port);
    // if(errno != 0)
    // {
    //     perror("Error in setting up target ipv4 address.");
    //     socket_fd = -1;
    //     goto done;
    // }
    printf("setup socket successfully");
done:
    return socket_fd;
}

void handle_peer(struct network_socket *data, const game *g, player *local_player, player *other_player, int *err)
{
    move_function_p move_func;
    char            error_message[TEMPHUNNO];

    socklen_t addr_len = sizeof(*(data->dest_ipv4_addr));
    uint8_t  *buffer   = new_player_buffer(local_player, err);
    printf("1");
    set_move_function(g, &move_func);

    if(signal(SIGINT, handle_signal) == SIG_ERR)
    {
        perror("Error setting up signal handler");
        return;
    }
    printf("2");

    // Server loop
    while(terminate != 1)
    {
        ssize_t bytes_read;
        ssize_t bytes_written;
        int     original_x;
        int     original_y;

        // Clear buffer to for reading
        memset(buffer, 0, sizeof(local_player->id) + sizeof(local_player->x) + sizeof(local_player->y));

        bytes_read = recvfrom(data->socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)data->dest_ipv4_addr, &addr_len);
        if(bytes_read > 0)
        {    // If data found
            // Clears previous other_player location
            mvaddch((int)other_player->y, (int)other_player->x, ' ');

            memcpy(other_player, buffer, sizeof(*other_player));
            printf("Packet id: %u", other_player->id);
            printf("x: %u", other_player->x);
            printf("y: %u", other_player->y);

            // Sets the new coordinates of other_player
            move_func(g, other_player, err);
            // Displays other_player at the new location
            mvaddch((int)other_player->y, (int)other_player->x, '*');
            refresh();
        }
        else
        {
            if(errno == EWOULDBLOCK)
            {
                // no data available, try again later
                goto send_data;
            }
            perror("Error receiving other player data.");
            *err = errno;
            goto cleanup;
        }
    send_data:
        // Clear buffer for writing
        memset(buffer, 0, sizeof(local_player->id) + sizeof(local_player->x) + sizeof(local_player->y));

        original_x = (int)local_player->x;
        original_y = (int)local_player->y;

        // Sets the new coordinates of local_player
        move_func(g, local_player, err);
        // Clears original local_player location
        mvaddch(original_y, original_x, ' ');
        // Displays local_player at the new location
        mvaddch((int)local_player->y, (int)local_player->x, '*');
        refresh();

        // Serialize local player, to prepare for data sending
        serialize_player(local_player, buffer);

        bytes_written = sendto(data->socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)data->dest_ipv4_addr, addr_len);
        if(bytes_written == -1)
        {
            if(errno == EWOULDBLOCK)
            {
                // Retry later, perhaps with a delay
                mvprintw(TEMPA, TEMPA, "BLOCKED! continue");
                continue;
            }
            // TODO: Must fix "Address family not supported by protocol."
            snprintf(error_message, sizeof(error_message), "Error sending local player data: %d", errno);
            perror(error_message);
            // perror("Error sending local player data.");
            *err = errno;
            goto cleanup;
        }
    }
    printf("3");
cleanup:
    free(buffer);
}

void close_socket(int socket_fd)
{
    if(close(socket_fd) == -1)
    {
        perror("Error closing socket");
        exit(EXIT_FAILURE);
    }
}
