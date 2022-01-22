#include "node.h"
#include "app.h"

#include <cmath>
#include <iostream>

Node::Node()
{
    depthLimit_ = 0;
    depth_ = 0;
    path_ = std::filesystem::path();
    isDirectory_ = std::filesystem::is_directory(path_);
}

Node::Node(std::filesystem::path path, int depthLimit, int depth, Node *parent, App *app)
{
    depthLimit_ = depthLimit;
    depth_ = depth;
    path_ = path;
    isDirectory_ = std::filesystem::is_directory(path_);
    parent_ = parent;
    app_ = app;
}

Node::~Node()
{
    if (isTarget_)
    {
        // If we delete the currently selected node, the selection should be reverted to the root. Notify App so that
        // this may be done.
        app_->notifyTargetDeletion();
    }
}

void Node::update(double t)
{
    accumulatedTime_ += t;

    Vector2 dist = Vector2Subtract(targetPos_, pos_);
    Vector2 move = Vector2Scale(dist, 2 * t);
    pos_ = Vector2Add(pos_, move);

    double totalChildWeight = 0.0001; // Very small value to avoid division by zero in case when there are no children.
    for (auto &child : children_)
        totalChildWeight += child->getWeight();

    // We need to compute children's target positions.
    double anglePerWeight = 2 * PI / totalChildWeight;

    double angle = accumulatedTime_ / 50.0;
    Vector2 base = {1, 0};

    for (auto &child : children_)
    {
        angle += anglePerWeight * child->getWeight() / 2.0;

        Vector2 tgtDirection = Vector2Rotate(base, angle);
        double scaler = pow((depth_ + 1), 2);
        child->setTargetPosition(Vector2Add(pos_, Vector2Scale(tgtDirection, BASE_CHILD_DISTANCE / scaler)));

        angle += anglePerWeight * child->getWeight() / 2.0;
    }

    for (auto &child : children_)
    {
        child->update(t);
    }
}

void Node::render()
{
    Color col = isDirectory_ ? BLUE : GREEN;

    int size = 1 + 2 * (4 - depth_);
    if (size < 1)
        size = 1;

    DrawCircle(pos_.x, pos_.y, size, col);

    if (depth_ <= MAX_DEPTH_RENDER_LABEL || isTarget_)
    {
        DrawText(path_.filename().string().c_str(), pos_.x + 13, pos_.y - 13, 12, WHITE);

        if (isTarget_)
            DrawCircleLines(pos_.x, pos_.y, 5 + size, RED);
    }

    for (auto &child : children_)
    {
        child->render();
    }
}

void Node::makeChildren()
{
    refreshDir();
}

void Node::deleteChildren()
{
    children_.clear();
}

void Node::setPosition(Vector2 pos)
{
    pos_ = pos;
}

void Node::setTargetPosition(Vector2 target)
{
    targetPos_ = target;
}

void Node::setAsTarget()
{
    isTarget_ = true;
}

void Node::removeAsTarget()
{
    isTarget_ = false;
}

std::vector<Node *> Node::getChildren()
{
    std::vector<Node *> children{children_.size()};

    for (int i = 0; i < children_.size(); i++)
    {
        children[i] = &*children_[i];
    }
    return children;
}

Node *Node::getParent()
{
    return parent_;
}

void Node::refreshDir()
{
    if (!std::filesystem::is_directory(path_) || depth_ + 1 >= depthLimit_)
    {
        children_.clear();
        return;
    }

    std::unordered_map<std::string, std::unique_ptr<Node>> currChildren;
    for (auto &c : children_)
    {
        // Steal ownership of children from unique_ptr.
        std::string s = c->path_.filename().string();
        currChildren[s] = std::unique_ptr<Node>(c.release());
    }

    std::vector<std::unique_ptr<Node>> newChildren;
    std::filesystem::directory_iterator dir{path_};
    for (auto &entry : dir)
    {
        auto c = currChildren.find(entry.path().filename().string());

        if (c != currChildren.end())
        {
            // Child already exists, use it.
            newChildren.emplace_back(c->second.release());
        }
        else
        {
            // Make new child
            newChildren.push_back(std::make_unique<Node>(entry.path().string(), depthLimit_, depth_ + 1, this, app_));
            newChildren.back()->makeChildren();
        }
    }

    children_ = std::move(newChildren);

    for (auto &child : children_)
    {
        child->refreshDir();
    }

    computeWeight();
}

void Node::computeWeight()
{
    if (children_.size() < WEIGHT_TABLE_SIZE)
        weight_ = WEIGHT_TABLE[children_.size()];
    else
        weight_ = WEIGHT_LARGE;

    // Boost for very large children.
    for (auto &c : children_)
        if (c->getWeight() >= WEIGHT_LARGE)
            weight_ = WEIGHT_LARGE + 1.0;
}

double Node::getWeight()
{
    return weight_;
}

std::unique_ptr<Node> Node::unParent()
{
    if (parent_ != nullptr)
    {
        for (auto c = parent_->children_.begin(); c != parent_->children_.end(); c++)
        {
            if (&**c == this)
            {
                std::unique_ptr<Node> out;
                out = std::unique_ptr<Node>(c->release());
                parent_->children_.erase(c);
                parent_ = nullptr;
                return out;
            }
        }
    }

    return std::unique_ptr<Node>(nullptr);
}

void Node::updateDepthAndLimit(int newDepth, int newDepthLimit)
{
    depthLimit_ = newDepthLimit;
    depth_ = newDepth;

    for (auto &c : children_)
    {
        c->updateDepthAndLimit(newDepth + 1, newDepthLimit);
    }
}

std::filesystem::path Node::getPath()
{
    return path_;
}