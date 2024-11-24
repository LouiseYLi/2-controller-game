#ifndef GAME_NETWORK_UTILS_H
#define GAME_NETWORK_UTILS_H

#include "gui.h"
#include "input_processing.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Here I ignored the warning for terminate because I wanted
//  terminate to act as a global flag for handling SIGINT.
//  I couldn't think of an alternative to not using a
//  non-constant global flag that also avoids compiler
//  warnings.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static int terminate = 0;
#pragma GCC diagnostic pop

// TODO: ensure each packet delivered has packet id
// TODO: when receiving, check if new packet_id is > than current
//       packet_id. if >, then continue to process, else ignore it
// static int packet_id = 0;

// Compiler thinks that I'm not using the struct members because the struct
//  is in a separate file.
// I made struct in a separate file to import because I didn't want to
//  declare it multiple times for the program, that's why I disabled the
//  unusedStructMember warnings.
struct network_socket
{
    // cppcheck-suppress unusedStructMember
    char *src_ip;
    // cppcheck-suppress unusedStructMember
    char *dest_ip;
    // cppcheck-suppress unusedStructMember
    struct sockaddr_in *src_ipv4_addr;
    // cppcheck-suppress unusedStructMember
    struct sockaddr_in *dest_ipv4_addr;
    // cppcheck-suppress unusedStructMember
    in_port_t port;
    // cppcheck-suppress unusedStructMember
    int socket_fd;
};

void handle_signal(int signal);

in_port_t convert_port(const char *str, int *err);

struct sockaddr_in *setup_network_address(struct sockaddr_storage *addr, socklen_t *addr_len, const char *address, in_port_t port, int *err);

void set_socket_flags(int socket_fd, int *err);

void bind_network_socket(int socket_fd, const void *addr, socklen_t addr_len, int *err);

int setup_network_socket(struct network_socket *data, const char *address, in_port_t port, int *err);

void handle_peer(struct network_socket *data, const game *g, player *local_player, player *other_player, int *err);

void close_socket(int socket_fd);

#endif    // GAME_NETWORK_UTILS_H
