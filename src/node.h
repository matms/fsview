#ifndef NODE_H
#define NODE_H

#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <string>
#include <filesystem>

class App; // Allow reference to app.

const int WEIGHT_TABLE_SIZE = 7;
const double WEIGHT_TABLE[WEIGHT_TABLE_SIZE] = {1.0, 3.0, 3.1, 3.2};
const double WEIGHT_LARGE = 3.6;

const int DEFAULT_DEPTH_LIMIT = 5;    // recommended: 5
const int MAX_DEPTH_RENDER_LABEL = 1; // recommended: 1
const double BASE_CHILD_DISTANCE = 380.0;

/** A node representing a file or directory entry in a tree structure. 
 * If it is a directory node, also owns all children node, i.e. this class is responsible for the entire subtree.
 */
class Node
{
private:
    int depthLimit_;
    int depth_;
    Vector2 pos_ = {0, 0};
    Vector2 targetPos_ = {0, 0};
    std::vector<std::unique_ptr<Node>> children_{};
    std::filesystem::path path_;
    bool isDirectory_;
    bool isTarget_ = false;
    double accumulatedTime_ = 0;
    Node *parent_ = nullptr;
    App *app_ = nullptr;
    double weight_ = 1;

public:
    /** Create a new node. */
    Node();
    /** Create a new node. */
    Node(std::filesystem::path path, int depthLimit, int depth, Node *parent, App *app);

    /** Node destructor. Responsible for notifying App if target node is deleted. */
    ~Node();

    /** Create this node's children, assuming this is a directory node, rather than a file. */
    void makeChildren();
    /** Delete all of this node's children. */
    void deleteChildren();

    /** Re-read directory information, so as to update the tree with any additions / deletions / changes. */
    void refreshDir();

    /** Perform the per-frame update for this node and all (recursively) of its children. */
    void update(double t);
    /** Draw this node and all of its children. */
    void render();

    /** Immediately "teleport" this node to a position. */
    void setPosition(Vector2 pos);
    /** Set the target position for this node. The node will move to that position over time, creating a smooth movement
     *  "animation". */
    void setTargetPosition(Vector2 target);

    /** Notify this node that it is the target node, i.e. the currently selected node by the cursor. */
    void setAsTarget();
    /** Remove this node as the target node. */
    void removeAsTarget();

    /** Return a vector of pointers to all of this node's children, in order */
    std::vector<Node *> getChildren();
    /** Return a pointer to this node's parent, or nullptr if this node is the root node. */
    Node *getParent();

    /* Compute the weight of this node. */
    void computeWeight();

    /** Return the (cached) weight of this Node, which has to do with how many children this node has. This is used so
     * that nodes with many children get more space in the display, and nodes with few children are more tightly packed.
     */
    double getWeight();

    /** Remove this node as a child of this nodes' parent, and return ownership of self to the caller. 
     * Returns a null unique pointer if called on node without parent. */
    std::unique_ptr<Node> unParent();

    /** Change a node's depth and depth limit. You likely should call refreshDir in succession.*/
    void updateDepthAndLimit(int newDepth, int newDepthLimit);

    /** Return the path to this node. */
    std::filesystem::path getPath();
};

#endif