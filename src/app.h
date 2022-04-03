#pragma once

#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "logic.h"

static struct sigaction sa;

class App {
public:
    App() = default;
    ~App() = default;

    void initialize_signal_handlers();
    void mainloop();

private:
    void initialize_socket();
    void teardown_socket();
    void data_flow_loop();

    Logic logic {};

    int down_flag = 0;
    struct sockaddr_un name;
    int connection_socket;
    int data_socket;
    uint8_t buffer[2];
    int16_t input_num = 10000;
};