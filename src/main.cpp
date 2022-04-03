#include "app.h"

int main(int argc, char* argv[])
{
    App app {};
    app.initialize_signal_handlers();
    app.mainloop();
    return 0;
}
