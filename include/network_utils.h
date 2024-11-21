#ifndef GAME_NETWORK_UTILS_H
#define GAME_NETWORK_UTILS_H

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
    in_port_t port;
    // cppcheck-suppress unusedStructMember
    int socket_fd;
};

in_port_t convert_port(const char *str, int *err);

void setup_network_address(struct sockaddr_storage *addr, socklen_t *addr_len, const char *address, in_port_t port, int *err);

void set_socket_flags(int socket_fd, int *err);

void bind_network_socket(int socket_fd, const void *addr, socklen_t addr_len, int *err);

int setup_network_socket(const char *address, in_port_t port, int *err);

void close_socket(int socket_fd);

#endif    // GAME_NETWORK_UTILS_H
