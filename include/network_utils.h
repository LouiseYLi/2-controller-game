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

in_port_t convert_port(const char *str, int *err);

// int open_network_socket_client(const char *address, in_port_t port, int *err);

// int open_network_socket_server(const char *address, in_port_t port, int backlog, int *err);

// int accept_connection(int server_fd, int *err);

#endif    // GAME_NETWORK_UTILS_H
