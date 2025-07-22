#ifndef DNODE_HPP
#define DNODE_HPP

#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include "archive/archive.hpp"

// for trees with multiple node types, maybe have some kind of type branch class? 
//   i feel like that is getting unnecessarily complicated
//   if there's a way to have different classes in the tree, keep that functionality in Thingy. 
//   its not necessary for every kind of node

template<typename T, typename Container>
class Node {
public:
    using NodePtr = std::shared_ptr<T>;

    virtual ~Node() = default;

    virtual NodePtr getParent() const {
        if (auto p = parent.lock())
            return p;
        return nullptr;
    };

    virtual void setParent(NodePtr newParent) {};

    virtual void addChild(NodePtr child) {};
    //
    // void removeChild(std::shared_ptr<Node> child);
    // void removeChild(Node* child);

protected:
    std::weak_ptr<T> parent;
    Container children;
    
};

#endif
