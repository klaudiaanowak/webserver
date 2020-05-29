#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#define BUFFER_SIZE 10000000	// 10 MB
#define ERROR(str)                                         \
    {                                                      \
        fprintf(stderr, "%s: %s\n", str, strerror(errno)); \
        exit(EXIT_FAILURE);                                \
    }
// void send_response(int sockfd, u_int8_t *buffer, size_t n);
int create_response(int sockfd, u_int8_t *recv_buffer, char *response_text);
int receive_request(int client_fd, int client_fd2, u_int8_t *recv_buffer );

// ssize_t recv_utill(int fd, u_int8_t *buffer, size_t buffer_size, unsigned int timeout);