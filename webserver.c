#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include "request_handling.h"

u_int8_t recv_buffer[BUFFER_SIZE + 1];

int main(int argc, char *argv[])
{
    char dir_path[300];
    int port;

    // Check input data
    if (argc != 3)
    {
        ERROR("Usage: ./webserver port directory\n");
    }
    // Assign input data
    port = atoi(argv[1]);
    strcpy(dir_path, argv[2]);

    int one = 1;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        ERROR("socket error");

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
        ERROR("bind error");

    if (listen(sockfd, 64) < 0)
        ERROR("listen error");
    struct sockaddr_in server_address2;

    // Go to given directory
    if (0 != access(dir_path, F_OK))
    {
        if (ENOENT == errno)
        {
            // does not exist
            ERROR("Access error");
        }
        if (ENOTDIR == errno)
        {
            // not a directory
            ERROR("Access error");
        }
    }
    chdir(dir_path);

    for (;;)
    {

        int connected_sockfd = accept(sockfd, NULL, NULL);
        if (connected_sockfd < 0)
            ERROR("accept error");

        fd_set descriptors;
        FD_ZERO(&descriptors);
        FD_SET(connected_sockfd, &descriptors);
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        int alive = 1;
        fcntl(connected_sockfd, F_SETFL, O_NONBLOCK);

        while (alive)
        {
            alive = select(connected_sockfd + 1, &descriptors, NULL, NULL, &tv);
            if (alive == -1)
            {
                ERROR("select error");
            }

            int bytes_recieved;
            while ((bytes_recieved = recv(connected_sockfd, recv_buffer, BUFFER_SIZE, 0)) > 0)
            {

                recv_buffer[bytes_recieved] = 0;
                char mess[900000] = {};
                // create response
                int send = create_response(connected_sockfd, recv_buffer, mess);

                if(send == 0)
                    alive = 0;
                else
                {
                    alive = 1;
  
                }

            }
            if (!bytes_recieved)
                break;
        }
        if (close(connected_sockfd) < 0)
            ERROR("close error");
    }
    if (close(sockfd) < 0)
        ERROR("close error");
}