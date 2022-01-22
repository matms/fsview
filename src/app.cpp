#include "app.h"

#include "node.h"
#include <cmath>
#include <iostream>

App::App(char *custom_path)
{
    init(std::filesystem::absolute(custom_path ? std::filesystem::path(custom_path) : std::filesystem::current_path()));
}

void App::init(std::filesystem::path path)
{
    visualizationRoot_ = path;
    root_ = std::make_unique<Node>(visualizationRoot_, DEFAULT_DEPTH_LIMIT, 0, nullptr, this);
    selection_ = &*root_;
    root_->setAsTarget();

    root_->setPosition({(rand() % 2) ? -300.0f : 300.0f, (rand() % 2) ? -300.0f : 300.0f});
    root_->makeChildren();
    root_->setTargetPosition({0, 0});
}

void App::run()
{
    std::cout << visualizationRoot_ << std::endl;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(windowWidth_, windowHeight_, "fsview");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        windowWidth_ = GetScreenWidth();
        windowHeight_ = GetScreenHeight();
        centerCamera();
        raylibCamera_.zoom = fmin((double)windowHeight_ / (double)DEFAULT_WINDOW_HEIGHT, (double)windowWidth_ / (double)DEFAULT_WINDOW_WIDTH);

        handleInput();

        root_->update(GetFrameTime());

        BeginDrawing();
        BeginMode2D(raylibCamera_);

        ClearBackground((Color){25, 25, 25, 255});

        root_->render();

        EndMode2D();
        EndDrawing();
    }

    CloseWindow();
}

void App::centerCamera()
{
    raylibCamera_.offset = {(float)windowWidth_ / 2,
                            (float)windowHeight_ / 2};
}

void App::handleInput()
{
    if (IsKeyPressed(KEY_U))
    {
        root_->refreshDir();
    }

    if (IsKeyPressed(KEY_X))
    {
        if (selection_ != &*root_)
        {
            root_ = selection_->unParent();
            root_->setTargetPosition({0, 0});
            root_->updateDepthAndLimit(0, DEFAULT_DEPTH_LIMIT);
            root_->refreshDir();
        }
    }

    if (IsKeyPressed(KEY_Z))
    {
        init(root_->getPath().parent_path());
    }

    if (IsKeyPressed(KEY_C) || IsKeyPressed(KEY_DOWN))
    {
        auto children = selection_->getChildren();
        if (children.size() > 0)
        {
            selection_->removeAsTarget();
            children[0]->setAsTarget();
            selection_ = children[0];
        }
    }

    if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_UP))
    {
        auto parent = selection_->getParent();
        if (parent != nullptr)
        {
            selection_->removeAsTarget();
            parent->setAsTarget();
            selection_ = parent;
        }
    }

    if (IsKeyPressed(KEY_L) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_R) || IsKeyPressed(KEY_RIGHT))
    {
        auto parent = selection_->getParent();
        if (parent != nullptr)
        {
            auto siblings = parent->getChildren();
            if (siblings.size() >= 2)
            {
                // Find the selection in the siblings array, to be able to go to the previous one.
                auto selInSiblings = std::find(siblings.begin(), siblings.end(), selection_);

                if (IsKeyPressed(KEY_L) || IsKeyPressed(KEY_LEFT))
                {
                    if (selInSiblings > siblings.begin())
                    {
                        selInSiblings--;
                        selection_->removeAsTarget();
                        (*selInSiblings)->setAsTarget();
                        selection_ = *selInSiblings;
                    }
                    else if (selInSiblings == siblings.begin()) // Wrap around
                    {
                        selection_->removeAsTarget();
                        siblings.back()->setAsTarget();
                        selection_ = siblings.back();
                    }
                }
                else if (IsKeyPressed(KEY_R) || IsKeyPressed(KEY_RIGHT))
                {
                    if (selInSiblings + 1 < siblings.end())
                    {
                        selInSiblings++;
                        selection_->removeAsTarget();
                        (*selInSiblings)->setAsTarget();
                        selection_ = *selInSiblings;
                    }
                    else if (selInSiblings + 1 == siblings.end()) // Wrap around
                    {
                        selection_->removeAsTarget();
                        siblings.front()->setAsTarget();
                        selection_ = siblings.front();
                    }
                }
            }
        }
    }
}

void App::notifyTargetDeletion()
{
    selection_ = &*root_;
    selection_->setAsTarget();
}