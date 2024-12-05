#include "../include/network_utils.h"
#include "../include/gui.h"
#include <ncurses.h>
#include <stdio.h>

#define ERR_NONE 0
#define ERR_NO_DIGITS 1
#define ERR_OUT_OF_RANGE 2
#define ERR_INVALID_CHARS 3
#define DISCONNECT_CODE 1000
#define PACKET_SIZE 5

#define sleep_time 20000000
#define recv_sleep_time 10000000

// #define TEMPB 35


pthread_mutex_t* get_player_lock(void) {
    static pthread_mutex_t player_lock = PTHREAD_MUTEX_INITIALIZER;
    return &player_lock;
}


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

void get_peer_address_to_host(struct sockaddr_storage *addr, in_port_t port)
{
    if(addr->ss_family == AF_INET)
    {
        struct sockaddr_in *ipv4_addr;

        ipv4_addr             = (struct sockaddr_in *)addr;
        ipv4_addr->sin_family = AF_INET;
        ipv4_addr->sin_port   = htons(port);
    }
    else if(addr->ss_family == AF_INET6)
    {
        struct sockaddr_in6 *ipv6_addr;

        ipv6_addr              = (struct sockaddr_in6 *)addr;
        ipv6_addr->sin6_family = AF_INET6;
        ipv6_addr->sin6_port   = htons(port);
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
    if(result == -1)
    {
        close(socket_fd);
        *err = errno;
        return;
    }
}

int socket_bind(int sockfd, struct sockaddr_storage *addr, in_port_t port)
{
    char      addr_str[INET6_ADDRSTRLEN];
    socklen_t addr_len;
    void     *vaddr;
    in_port_t net_port;
    int bind_res;

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
        close(sockfd);
        fprintf(stderr, "Internal error: addr->ss_family must be AF_INET or AF_INET6, was: %d\n", addr->ss_family);
        exit(EXIT_FAILURE);
    }

    if(inet_ntop(addr->ss_family, vaddr, addr_str, sizeof(addr_str)) == NULL)
    {
        close(sockfd);
        perror("inet_ntop");
        exit(EXIT_FAILURE);
    }

    printf("Binding to: %s:%u\n", addr_str, port);

    bind_res = bind(sockfd, (struct sockaddr *)addr, addr_len);

    if(bind_res == -1)
    {
        close(sockfd);
        perror("Binding failed");
        fprintf(stderr, "Error code: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    printf("Bound to socket: %s:%u\n", addr_str, port);

    return sockfd;
}

int setup_host_socket(struct network_socket *data, int *err)
{
    int bind_res;
    struct sockaddr_storage addr;
    // Convert host address
    convert_address(data->src_ip, &addr);
    data->socket_fd = socket_create(addr.ss_family, SOCK_DGRAM, 0);
    set_socket_flags(data->socket_fd, err);
    if(*err != 0)
    {
        perror("Error setting socket flag to non-blocking.");
        data->socket_fd = -1;
        return -1;
    }
    bind_res = socket_bind(data->socket_fd, &addr, data->port);
    if(bind_res == -1)
    {
        perror("Error binding socket.");
        close(data->socket_fd);
        data->socket_fd = -1;
        return -1;
    }

    return bind_res;
}

struct thread_data
{
    struct network_socket *data;
    const game           *g;
    player               *local_player;
    player               *other_player;
    int                  *err;
};

void handle_peer(struct network_socket *data, const game *g, player *local_player, player *other_player, int *err)
{
    struct thread_data *t_data;
    pthread_t ptid;
    pthread_t stid;
    

    if(signal(SIGINT, handle_signal) == SIG_ERR)
    {
        perror("Error setting up signal handler");
        *err = -1;
        return;
    }

    // Convert peer address and set peer to host
    convert_address(data->dest_ip, &data->peer_addr);
    get_peer_address_to_host(&data->peer_addr, data->port);

    data->peer_addr_len = sizeof(data->peer_addr);

    t_data = (struct thread_data *)malloc(sizeof(struct thread_data));
    

    if(t_data == NULL) {
        perror("Error allocating memory for thread data.");
        *err = errno;
        return;
    }
        t_data->data         = data;
        t_data->g            = g;
        t_data->local_player = local_player;
        t_data->other_player = other_player;
        t_data->err          = err;

    if (pthread_create(&ptid, NULL, receive_thread_func, t_data) != 0) {
        perror("Error creating thread");
        free(t_data);
        *err = errno;
        return;
    }

    if (pthread_create(&stid, NULL, send_thread_func, t_data) != 0) {
        perror("Error creating thread");
        free(t_data);
        *err = errno;
        return;
    }

    // Server loop
    while(terminate != 1)
    {
        mvaddch((int)other_player->prev_y, (int)other_player->prev_x, ' ');
        mvaddch((int)other_player->y, (int)other_player->x, '*');
        mvaddch((int)local_player->prev_y, (int)local_player->prev_x, ' ');
        mvaddch((int)local_player->y, (int)local_player->x, '*');
        refresh();
    }
    pthread_join(ptid, NULL);
    pthread_join(stid, NULL);
    free(t_data);
    endwin();
}

void *send_thread_func(void *arg) 
{
    struct thread_data *data = (struct thread_data *)arg;
    uint32_t host_buffer[PACKET_SIZE];
    struct timespec req;
    struct timespec rem;
    uint32_t disconnect_message[3];
    move_function_p move_func;
    req.tv_sec                  = 0;
    req.tv_nsec                 = sleep_time;

    set_move_function(data->g, &move_func);
    if(move_func == NULL)
    {
        perror("Error setting move function");
        *data->err = -1;
        return NULL;
    }

    while(terminate != 1) 
    {

        nanosleep(&req, &rem);

        data->local_player->prev_x = data->local_player->x;
        data->local_player->prev_y = data->local_player->y;
    
        move_func(data->g, data->local_player, data->err);
        if(data->local_player->prev_x != data->local_player->x || data->local_player->prev_y != data->local_player->y)
        {
            ssize_t bytes_sent;
            // ++data->data->current_seq_num;
            host_buffer[0] = data->local_player->x;
            host_buffer[1] = data->local_player->y;
            host_buffer[2] = data->data->current_seq_num;
            host_buffer[3] = data->local_player->prev_x;
            host_buffer[4] = data->local_player->prev_y;

            bytes_sent = sendto(data->data->socket_fd, host_buffer, sizeof(host_buffer), 0, (struct sockaddr *)&data->data->peer_addr, data->data->peer_addr_len);
            if(bytes_sent < 0 && errno != EWOULDBLOCK)
            {
                perror("Error while sending data.");
                *data->err = errno;
            }
        }
    }

        disconnect_message[0] = DISCONNECT_CODE;
        disconnect_message[1] = DISCONNECT_CODE;
        disconnect_message[2] = data->data->current_seq_num;
        if(sendto(data->data->socket_fd, disconnect_message, sizeof(disconnect_message), 0, (struct sockaddr *)&data->data->peer_addr, data->data->peer_addr_len) == -1)
        {
            perror("Error sending disconnect code");
            *data->err = errno;
        }
        printf("Sent disconnect message to peer\n");
    

    return NULL;
}

void *receive_thread_func(void *arg)
{
    struct thread_data *data = (struct thread_data *)arg;
    uint32_t client_buffer[PACKET_SIZE];
    struct timespec req;
    struct timespec rem;
    req.tv_sec                  = 0;
    req.tv_nsec                 = recv_sleep_time;
    while(terminate != 1) {
        ssize_t bytes_received;

        nanosleep(&req, &rem);
        data->other_player->prev_x = data->other_player->x;
        data->other_player->prev_y = data->other_player->y;

        bytes_received = recvfrom(data->data->socket_fd, client_buffer, sizeof(client_buffer), 0, (struct sockaddr *)&data->data->peer_addr, &data->data->peer_addr_len);
        if(bytes_received > 0)
        {
            if(client_buffer[0] == (uint32_t)DISCONNECT_CODE && client_buffer[1] == (uint32_t)DISCONNECT_CODE)
            {
                terminate = 1;
                close(data->data->socket_fd);
                printf("Received disconnect message from peer\n");
            }

            // if(client_buffer[2] <= data->data->current_seq_num)
            // {
            
            //     continue;
            // }

            data->other_player->y       = client_buffer[1];
            data->other_player->x       = client_buffer[0];
            data->data->current_seq_num = client_buffer[2];
            data->other_player->prev_x  = client_buffer[3];
            data->other_player->prev_y  = client_buffer[4];
            
        }
        else if(errno != EWOULDBLOCK)
        {
            perror("Error while receiving data.");
            data->err = &errno;
        }
    }
    
    return NULL;
}

void close_socket(int socket_fd)
{
    if(close(socket_fd) == -1)
    {
        perror("Error closing socket");
        exit(EXIT_FAILURE);
    }
}
