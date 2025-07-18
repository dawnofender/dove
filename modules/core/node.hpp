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



class Node : public Thing {
CLASS_DECLARATION(Node)
protected:
    std::weak_ptr<Node> parent;
public: 
    std::vector<std::shared_ptr<Node>> children;

public:
    virtual ~Node();
    
    void serialize(Archive& archive) override;


    // addChild creates an object of the specified Node-derived type, and adds it as a child to this node.
    // Output: a reference to the new child node.
    // Inputs: arguments for the new node's constructor - varies by type.
    // Example: node.addChild<Window>("window") creates a child node of type Window and returns a reference to it.
    template<class NodeType, typename... Args>
    NodeType& addChild(Args&&... args);

    // addChild(shared_ptr<Node>) places the given node under this one as a child.
    // Output: N/A
    // Inputs: arguments for the new node's constructor - varies by type.
    // Example: node.addChild<Window>("window") creates a child node of type Window and returns a reference to it.
    void addChild(std::shared_ptr<Node> node);
    
    // getChild finds a child node of the given type.
    // Output: a reference to the found node.
    // Inputs: N/A
    // Example: node.getChild<Window>() may return a reference to a child of type Window.
    template<class NodeType>
    NodeType& getChild();

    Node& getRoot();
    

    void setParent(std::shared_ptr<Node> node);
    std::string getName();

    // not implemented yet (2)
    template<class NodeType>
    std::vector<NodeType*> getChildren();

    void removeChild(std::shared_ptr<Node> child);
    void removeChild(Node* child);

private:
    mutable Node* cachedRoot = nullptr; // mutable to allow caching in const getRoot()
    
};

template< class NodeType, typename... Args >
NodeType& Node::addChild(Args&&... args) {
    auto child = std::make_shared<NodeType>(std::forward<Args>(args)...);
    NodeType& ref = *child;
    return ref;
}


#endif
