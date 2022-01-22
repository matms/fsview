#include "app.h"
#include <memory>

int main(int argc, char **argv)
{
    std::unique_ptr<App> app;

    if (argc >= 2)
        app = std::make_unique<App>(argv[1]);
    else
        app = std::make_unique<App>();

    app->run();

    return 0;
}