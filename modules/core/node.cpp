#include "node.hpp"
#include <algorithm>
#include <vector>
#include "archive/archive.hpp"

CLASS_DEFINITION(Thing, Node)

void Node::serialize(Archive& archive) {
    archive & parent & children;
}

Node::~Node() {

    for ( auto&& child : children ) {
        //move children 
        if (auto p = parent.lock()) {
            p->addChild(child);
        } else {
            std::cerr << "Node: Destructor error: Failed to lock parent ptr, children deleted" << std::endl;
            child.reset();
            // handle whatever happens if locking the parent ptr doesnt work
        }
    }
}

void Node::addChild(std::shared_ptr<Node> child) {
    children.push_back(child);
    
    // if our node doesn't have a shared ptr to it, shared_from_this will fail
    if (!weak_from_this().expired()) {
        child->parent = new_shared_from_this();
    } else {
        std::cerr << "Node: addChild error: Node does not have a shared_ptr to it, cannot set child's parent" << std::endl;
    }
}
        
void Node::removeChild(Node* node) {
    auto it = std::find_if(children.begin(), children.end(),
        [node](const std::shared_ptr<Node>& child) {
            return child.get() == node;
        });
    
    if (it != children.end()) {
        children.erase(it);  // This will destroy the shared_ptr
    }
}

void Node::setParent(std::shared_ptr<Node> newParent) {
    newParent->addChild(new_shared_from_this());
}


// TODO: cached root
Node& Node::getRoot() {
    if (auto p = parent.lock())
        return p->getRoot();
    else 
        return *this;
}
