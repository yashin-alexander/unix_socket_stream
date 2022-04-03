#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>

#include "logic.h"

const char* SOCKET_NAME = "/tmp/example";
const int MSG_SIZE = sizeof(int16_t);


static struct sigaction sa;


void signal_handler(int signum){
    sigaction(SIGINT, &sa, 0);
    std::cout << signum << " signal received, shutting down..." << std::endl;
}


int main()
{

    Logic *logic = new Logic();

    struct sockaddr_un name;
    int down_flag = 0;
    int ret;
    int connection_socket;
    int data_socket;
    uint8_t buffer[2];
    int16_t input_num = 10000;

    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, 0);

    /* Create local socket. */
    connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connection_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&name, 0, sizeof(name));
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);
    ret = bind(connection_socket, (const struct sockaddr *) &name, sizeof(name));
    if (ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    ret = listen(connection_socket, 20);
    if (ret == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    /* This is the main loop for handling connections. */
    for (;;) {
        std::cout << "Wait for incoming connection..." << std::endl;
        data_socket = accept(connection_socket, NULL, NULL);
        if (data_socket == -1) {
            if (errno == EINTR) {
                down_flag = true;
                break;
            }
            std::cout << "error in accept " << std::endl;
            perror("accept");
            exit(EXIT_FAILURE);
        }

        std::cout << "Got new connection..." << std::endl;
        for (;;) {
            int select_ret {};
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(data_socket, &rfds);
            select_ret = select(data_socket + 1, &rfds, nullptr, nullptr, nullptr);
            if ((select_ret == -1) and (errno == EINTR)) {
                down_flag = true;
                break;
            }

            ret = recv(data_socket, buffer, MSG_SIZE, MSG_WAITALL);
            if (ret == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            } else if (ret == 0) {
                std::cout << "Other side is gracefully disconnected" << std::endl;
                break;
            } else if (ret < MSG_SIZE) {
                /* Means that MSG_WAITALL was ignored because of signal or
                 * if client was disconnected
                 * */
                std::cout << "Incomplete data read, skipping last byte..." << std::endl;
                continue;
            }

            input_num = *(int16_t *)buffer;
            std::cout << "New data received: " << input_num << std::endl;
            logic->add_number(input_num);
        }
        close(data_socket);
        if (down_flag) {
            break;
        }
    }
    close(connection_socket);
    unlink(SOCKET_NAME);
    exit(EXIT_SUCCESS);
}
