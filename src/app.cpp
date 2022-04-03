#include "app.h"

#include <string.h>
#include <unistd.h>

namespace {
const char* SOCKET_NAME = "/tmp/example";
const int MSG_SIZE = sizeof(int16_t);
}

static void signal_handler(int signum)
{
    sigaction(SIGINT, &sa, 0);
    sigaction(SIGTERM, &sa, 0);
    std::cout << signum << " signal received, shutting down..." << std::endl;
}

void App::initialize_signal_handlers()
{
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, 0);
    sigaction(SIGTERM, &sa, 0);
}

void App::initialize_socket()
{
    int ret;
    /* Create local socket. */
    connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connection_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&name, 0, sizeof(name));
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);
    ret = bind(connection_socket, (const struct sockaddr*)&name, sizeof(name));
    if (ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    ret = listen(connection_socket, 20);
    if (ret == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

void App::mainloop()
{
    initialize_socket();
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
            break;
        }

        std::cout << "Got new connection..." << std::endl;
        data_flow_loop();

        close(data_socket);
        if (down_flag) {
            break;
        }
    }
    teardown_socket();
}

void App::data_flow_loop()
{
    int ret;
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
            std::cerr << "Error on recv" << std::endl;
            down_flag = true;
            break;
        } else if (ret == 0) {
            std::cout << "Other side is gracefully disconnected" << std::endl;
            break;
        } else if (ret < MSG_SIZE) {
            /* Means that MSG_WAITALL was ignored because of signal or
             * if client was disconnected
             * */
            std::cerr << "Incomplete data read, skipping last byte..." << std::endl;
            continue;
        }

        input_num = *(int16_t*)buffer;
        std::cout << "New data received: " << input_num << std::endl;
        logic.add_number(input_num);
    }
}

void App::teardown_socket()
{
    close(connection_socket);
    unlink(SOCKET_NAME);
}
