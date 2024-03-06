#ifndef CLIENT_H
#define CLIENT_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define _XOPEN_SOURCE 700
#define LOG_FILE_PATH "logs/server_log_%Y-%m-%d_%H-%M-%S.log"
#define SERVER_SOCKET_PATH "/tmp/server_socket"
#define BUFFER_SIZE 1024

void cleanup();
void handle_signal();
void setup_signal_handling();

#endif