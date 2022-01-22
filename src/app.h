#ifndef APP_H
#define APP_H

#include <raylib.h>
#include <filesystem>
#include <memory>
#include "node.h"

const int DEFAULT_WINDOW_WIDTH = 1200;
const int DEFAULT_WINDOW_HEIGHT = 1200;

class App
{
private:
    std::filesystem::path visualizationRoot_;
    std::unique_ptr<Node> root_;
    Node *selection_;

    int windowWidth_ = DEFAULT_WINDOW_WIDTH;
    int windowHeight_ = DEFAULT_WINDOW_HEIGHT;

    Camera2D raylibCamera_ = {{0, 0}, {0, 0}, 0.0f, 1.0f};

    /** Center the camera, taking into account the current screen size. */
    void centerCamera();

    /** Detect and handle user input. See Readme.md for a list of accepted
     * inputs */
    void handleInput();

    /** Initialize, or reinitialize, the App, to display a given path as root
     * of the visualization tree. */
    void init(std::filesystem::path path);

public:
    /** Create an instance of the fsview app given a path argument (passed in
     * through command line), or if no path is given (denoted by nullptr),
     * visualize the current path. */
    App(char *custom_path = nullptr);

    /** Run th app. */
    void run();

    /** Used by the Node destructor to handle the situation where the selected node is deleted 
     * (e.g. if you delete a file that you have targeted). */
    void notifyTargetDeletion();
};

#endif