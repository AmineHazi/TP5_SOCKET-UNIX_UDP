#ifndef SERVER_H
#define SERVER_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define _XOPEN_SOURCE 700
#define LOG_DIR "logs"
#define LOG_FILE_PATH LOG_DIR "/server_log_%Y-%m-%d_%H-%M-%S.log"
#define SERVER_SOCKET_PATH "/tmp/server_socket"
#define BUFFER_SIZE 1024

typedef struct {
    int sockfd;
    struct sockaddr_un client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len;
} thread_data_t;

void create_log_file_path();
void log_message_with_client_info();
void log_message(const char* message);
void cleanup();
void handle_signal();
void setup_signal_handling();
void* handle_client(void* arg);

#endif