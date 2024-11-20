#include "../include/network_utils.h"

#define ERR_NONE 0
#define ERR_NO_DIGITS 1
#define ERR_OUT_OF_RANGE 2
#define ERR_INVALID_CHARS 3

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
        *err = ERR_NO_DIGITS;
        goto done;
    }

    // Check for out-of-range errors
    if(val < 0 || val > UINT16_MAX)
    {
        *err = ERR_OUT_OF_RANGE;
        goto done;
    }

    // Check for trailing invalid characters
    if(*endptr != '\0')
    {
        *err = ERR_INVALID_CHARS;
        goto done;
    }

    port = (in_port_t)val;

done:
    return port;
}

void setup_network_address(struct sockaddr_storage *addr, socklen_t *addr_len, const char *address, in_port_t port, int *err)
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
    }
    else if(inet_pton(AF_INET6, address, &(((struct sockaddr_in6 *)addr)->sin6_addr)) == 1)
    {
        struct sockaddr_in6 *ipv6_addr;

        ipv6_addr            = (struct sockaddr_in6 *)addr;
        addr->ss_family      = AF_INET6;
        ipv6_addr->sin6_port = net_port;
        *addr_len            = sizeof(struct sockaddr_in6);
    }
    else
    {
        fprintf(stderr, "%s is not an IPv4 or an IPv6 address\n", address);
        *err = errno;
    }
}

int set_socket_flags(int server_fd, int *err)
{
    int flags;
    // Returns flags of socket
    flags = fcntl(server_fd, F_GETFL, 0);

    if(flags == -1)
    {
        close(server_fd);
        server_fd = -1;
        *err      = errno;
        goto done;
    }

    // Sets non-blocking flag to socket
    if(fcntl(server_fd, F_SETFL, flags | O_NONBLOCK))
    {
        close(server_fd);
        server_fd = -1;
        *err      = errno;
        goto done;
    }
done:
    return server_fd;
}

int open_network_socket(const char *address, in_port_t port, int backlog, int *err)
{
    struct sockaddr_storage addr;
    socklen_t               addr_len;
    int                     server_fd;
    int                     result;

    // I dereferenced addr to get the address of it
    setup_network_address(&addr, &addr_len, address, port, err);

    if(*err != 0)
    {
        server_fd = -1;
        goto done;
    }
    // ERROR HERE
    server_fd = socket(addr.ss_family, SOCK_DGRAM, 0);    // NOLINT(android-cloexec-socket)

    if(server_fd == -1)
    {
        *err = errno;
        goto done;
    }

    server_fd = set_socket_flags(server_fd, err);
    if(*err != 0)
    {
        goto done;
    }

    result = bind(server_fd, (const struct sockaddr *)&addr, addr_len);

    if(result == -1)
    {
        close(server_fd);
        server_fd = -1;
        *err      = errno;
        goto done;
    }
done:
    return server_fd;
}

// int bind_network_socket(...)
