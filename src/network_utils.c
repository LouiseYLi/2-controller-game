#include "../include/network_utils.h"
#include "../include/gui.h"
#include <ncurses.h>
#include <stdio.h>

#define ERR_NONE 0
#define ERR_NO_DIGITS 1
#define ERR_OUT_OF_RANGE 2
#define ERR_INVALID_CHARS 3

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

void convert_address(const char *address, struct sockaddr_storage *addr)
{
    memset(addr, 0, sizeof(*addr));

    if(inet_pton(AF_INET, address, &(((struct sockaddr_in *)addr)->sin_addr)) == 1)
    {
        addr->ss_family = AF_INET;
    }
    else if(inet_pton(AF_INET6, address, &(((struct sockaddr_in6 *)addr)->sin6_addr)) == 1)
    {
        addr->ss_family = AF_INET6;
    }
    else
    {
        fprintf(stderr, "%s is not an IPv4 or an IPv6 address\n", address);
        exit(EXIT_FAILURE);
    }
}

int socket_create(int domain, int type, int protocol)
{
    int sockfd;

    sockfd = socket(domain, type, protocol);

    if(sockfd == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    return sockfd;
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

void socket_bind(int sockfd, struct sockaddr_storage *addr, in_port_t port)
{
    char      addr_str[INET6_ADDRSTRLEN];
    socklen_t addr_len;
    void     *vaddr;
    in_port_t net_port;

    net_port = htons(port);

    if(addr->ss_family == AF_INET)
    {
        struct sockaddr_in *ipv4_addr;

        ipv4_addr           = (struct sockaddr_in *)addr;
        addr_len            = sizeof(*ipv4_addr);
        ipv4_addr->sin_port = net_port;
        vaddr               = (void *)&(((struct sockaddr_in *)addr)->sin_addr);
    }
    else if(addr->ss_family == AF_INET6)
    {
        struct sockaddr_in6 *ipv6_addr;

        ipv6_addr            = (struct sockaddr_in6 *)addr;
        addr_len             = sizeof(*ipv6_addr);
        ipv6_addr->sin6_port = net_port;
        vaddr                = (void *)&(((struct sockaddr_in6 *)addr)->sin6_addr);
    }
    else
    {
        fprintf(stderr, "Internal error: addr->ss_family must be AF_INET or AF_INET6, was: %d\n", addr->ss_family);
        exit(EXIT_FAILURE);
    }

    if(inet_ntop(addr->ss_family, vaddr, addr_str, sizeof(addr_str)) == NULL)
    {
        perror("inet_ntop");
        exit(EXIT_FAILURE);
    }

    printf("Binding to: %s:%u\n", addr_str, port);

    if(bind(sockfd, (struct sockaddr *)addr, addr_len) == -1)
    {
        perror("Binding failed");
        fprintf(stderr, "Error code: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    printf("Bound to socket: %s:%u\n", addr_str, port);
}

void setup_network_socket(struct network_socket *data, int *err)
{
    struct sockaddr_storage addr;
    convert_address(data->src_ip, &addr);
    data->socket_fd = socket_create(addr.ss_family, SOCK_DGRAM, 0);
    socket_bind(data->socket_fd, &addr, data->port);
    *err = errno;
    if(*err != 0)
    {
        perror("Error in setup_network_socket");
    }
}

// void handle_peer(struct network_socket *data, const game *g, player *local_player, player *other_player, int *err)
// {
// move_function_p move_func;

// socklen_t addr_len = sizeof(*(data->dest_ipv4_addr));
// uint8_t *buffer = new_player_buffer(local_player, err);
// printf("beginning of handle_peer\n");
// set_move_function(g, &move_func);

// if(signal(SIGINT, handle_signal) == SIG_ERR)
// {
//     perror("Error setting up signal handler");
//     return;
// }

// // Server loop
// while(terminate != 0)
// {
//     // ssize_t bytes_read;
//     // ssize_t bytes_written;
//     // int     original_x;
//     // int     original_y;

//     // char buffer[1];

//     // bytes_read = recvfrom();
//     // if (bytes_read > 0) {
//     //     printf("%c", buffer[0]);
//     // } else {
//     //     if (errno == EWOULDBLOCK) {
//     //         goto send_data;
//     //     }
//     // }

//     // send_data:
//     // bytes_written = sendto();
// }
// printf("successfully ran handle_peer\n");
// // cleanup:
// free(buffer);
// }

void close_socket(int socket_fd)
{
    if(close(socket_fd) == -1)
    {
        perror("Error closing socket");
        exit(EXIT_FAILURE);
    }
}
