#include <server.h>

char log_file_path[256]; 
int sockfd;

int main() {

    switch (fork()) {
        case 0:

            if(setsid() < 0){
                perror("setsid");
                exit(EXIT_FAILURE);
            }

            struct sockaddr_un server_addr, client_addr;
            char buffer[BUFFER_SIZE];
            memset(buffer, '\0', BUFFER_SIZE); 
            socklen_t addr_size;
            
            mkdir(LOG_DIR, 0755);
            setup_signal_handling();

            create_log_file_path();
            if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
                perror("socket error");
                exit(EXIT_FAILURE);
            }
            log_message("Socket created.");

            memset(&server_addr, 0, sizeof(server_addr));
            server_addr.sun_family = AF_UNIX;
            strncpy(server_addr.sun_path, SERVER_SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
            unlink(SERVER_SOCKET_PATH); 

            if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
                perror("bind error");
                close(sockfd);
                exit(EXIT_FAILURE);
            }
            log_message("Socket bound to address.");

            log_message("Server is waiting for messages...");

            while (1) {
                addr_size = sizeof(client_addr);
                ssize_t received = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &addr_size);
                if (received < 0) {
                    perror("recvfrom error");
                    log_message("Error receiving message.");
                    continue;
                }
                log_message("Message received from client.");

                thread_data_t* data = malloc(sizeof(thread_data_t));
                if (!data) {
                    perror("malloc error");
                    log_message("Error allocating memory for thread data.");
                    continue;
                }
                data->sockfd = sockfd;
                memcpy(&data->client_addr, &client_addr, sizeof(client_addr));
                memcpy(data->buffer, buffer, BUFFER_SIZE);
                data->addr_len = addr_size;

                pthread_t thread_id;
                if (pthread_create(&thread_id, NULL, handle_client, (void*)data) != 0) {
                    perror("pthread_create error");
                    log_message("Error creating thread.");
                    free(data);
                    continue;
                }

                pthread_detach(thread_id);
                memset(buffer, '\0', BUFFER_SIZE); 
            }

        default:
            break;
    }

    return EXIT_SUCCESS;
}

void create_log_file_path() {
    time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);
    strftime(log_file_path, sizeof(log_file_path), LOG_FILE_PATH, tm_now);
}

void log_message_with_client_info(const char* prefix, struct sockaddr_un* client_addr, const char* message) {
    FILE* log_file = fopen(log_file_path, "a");
    if (log_file != NULL) {
        time_t now = time(NULL);
        char formatted_time[64];
        strftime(formatted_time, sizeof(formatted_time), "%Y-%m-%d %H:%M:%S", localtime(&now));

        fprintf(log_file, "[%s] (%s) %s: %s\n", formatted_time, client_addr->sun_path, prefix, message);
        fclose(log_file);
    } else {
        perror("Failed to open log file");
    }
}

void log_message(const char* message) {
    FILE* log_file = fopen(log_file_path, "a");
    if (log_file != NULL) {
        fprintf(log_file, "%s\n", message);
        fclose(log_file); 
    } else {
        perror("Failed to open log file");
    }
}

void cleanup() {
    close(sockfd);
    unlink(SERVER_SOCKET_PATH);
    log_message("Server shutdown.");
}

void handle_signal() {
    cleanup();
    exit(EXIT_SUCCESS);
}

void setup_signal_handling() {
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask); 
    sa.sa_flags = 0; 

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigation");
        exit(EXIT_FAILURE);
    }
}

void* handle_client(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;

    data->buffer[data->addr_len] = '\0';

    log_message_with_client_info("Received message from client", &data->client_addr, data->buffer);

    if (sendto(data->sockfd, data->buffer, strlen(data->buffer), 0, (struct sockaddr*)&data->client_addr, sizeof(data->client_addr)) == -1) {
        perror("sendto");
        log_message("Error sending response to client.");
    } else {
        log_message_with_client_info("Response sent to client", &data->client_addr, "Response message");
    }

    free(data);
    pthread_exit(NULL);
}