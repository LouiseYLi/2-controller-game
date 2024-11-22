#include "../include/network_utils.h"

#define ERR_NONE 0
#define ERR_NO_DIGITS 1
#define ERR_OUT_OF_RANGE 2
#define ERR_INVALID_CHARS 3

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

struct sockaddr_in *setup_network_address(struct sockaddr_storage *addr, socklen_t *addr_len, const char *address, in_port_t port, int *err)
{
    in_port_t net_port;

    *addr_len = 0;
    net_port  = htons(port);
    memset(addr, 0, sizeof(*addr));

    if(inet_pton(AF_INET, address, &(((struct sockaddr_in *)addr)->sin_addr)) == 1)
    {
        struct sockaddr_in *ipv4_addr;

        ipv4_addr           = (struct sockaddr_in *)addr;
        addr->ss_family     = AF_INET;
        ipv4_addr->sin_port = net_port;
        *addr_len           = sizeof(struct sockaddr_in);
        return ipv4_addr;
    }
    // else if(inet_pton(AF_INET6, address, &(((struct sockaddr_in6 *)addr)->sin6_addr)) == 1)
    // {
    //     struct sockaddr_in6 *ipv6_addr;

    //     ipv6_addr            = (struct sockaddr_in6 *)addr;
    //     addr->ss_family      = AF_INET6;
    //     ipv6_addr->sin6_port = net_port;
    //     *addr_len            = sizeof(struct sockaddr_in6);
    //     return ipv6_addr;
    // }
    else
    {
        fprintf(stderr, "%s is not an IPv4 or an IPv6 address\n", address);
        *err = errno;
        return NULL;
    }
}

void set_socket_flags(int socket_fd, int *err)
{
    int flags;
    // Returns flags of socket
    flags = fcntl(socket_fd, F_GETFL, 0);

    if(flags == -1)
    {
        close(socket_fd);
        *err = errno;
        return;
    }

    // Sets non-blocking flag to socket
    if(fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK))
    {
        close(socket_fd);
        *err = errno;
        return;
    }
}

void bind_network_socket(int socket_fd, const void *addr, socklen_t addr_len, int *err)
{
    int result = bind(socket_fd, (const struct sockaddr *)addr, addr_len);

    if(result == -1)
    {
        close(socket_fd);
        *err = errno;
        return;
    }
}

int setup_network_socket(struct network_socket *data, const char *address, in_port_t port, int *err)
{
    struct sockaddr_storage addr;
    struct sockaddr_storage non_local_addr;
    socklen_t               addr_len;
    int                     socket_fd;

    // I dereferenced addr to get the address of it
    // Assigned local ipv4 addr to member in struct
    data->src_ipv4_addr = setup_network_address(&addr, &addr_len, address, port, err);
    // TODO: if null...

    // Set up non-local ipv4 addr and assign to struct
    data->dest_ipv4_addr = setup_network_address(&non_local_addr, &addr_len, address, port, err);
    // TODO: if null...

    if(*err != 0)
    {
        socket_fd = -1;
        goto done;
    }
    // ERROR HERE
    socket_fd = socket(addr.ss_family, SOCK_DGRAM, 0);    // NOLINT(android-cloexec-socket)

    if(socket_fd == -1)
    {
        *err = errno;
        goto done;
    }

    set_socket_flags(socket_fd, err);
    if(*err != 0)
    {
        socket_fd = -1;
        goto done;
    }

    bind_network_socket(socket_fd, &addr, addr_len, err);
    if(*err != 0)
    {
        socket_fd = -1;
        goto done;
    }
done:
    return socket_fd;
}

//TODO: this whole section
void handle_peer(struct network_socket *data, const game *g, const player *local_player, const player *other_player, int *err)
{
    ssize_t         bytes_read;
    uint8_t        *buffer   = new_player_buffer(local_player, err);
    socklen_t       addr_len = sizeof(data->dest_ipv4_addr);
    move_function_p move;
    if(signal(SIGINT, handle_signal) == SIG_ERR)
    {
        perror("Error setting up signal handler");
        return;
    }
    while(terminate != 0)
    {
        bytes_read = 0;
        bytes_read = recvfrom(data->socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)data->dest_ipv4_addr, &addr_len);
        if(bytes_read > 0)
        {
            player temp_player;
            memcpy(&temp_player, buffer, sizeof(temp_player));
            printf("Packet id: %u", temp_player.id);
            printf("x: %u", temp_player.x);
            printf("y: %u", temp_player.y);
            // assign other_player with temp_player

            set_move_function(g, move);
            move(g, other_player, err);

            // send local_player to socket
            if(!sendto(socket_fd, buffer, sizeof(buffer), 0, (sockaddr)data->src_ip, sizeof((sockaddr)data->src_ip)))
            {
                perror("Error sending data.");
                *err = errno;
            }
        }
    }
}

void close_socket(int socket_fd)
{
    if(close(socket_fd) == -1)
    {
        perror("Error closing socket");
        exit(EXIT_FAILURE);
    }
}
