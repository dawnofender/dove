#include "thingy.hpp"
#include <algorithm>
#include <vector>


Thingy::~Thingy() {

    for ( auto&& child : children ) {
        //move children 
        if (auto p = parent.lock()) {
            p->addChild(child);
            child.reset();
        } else {
            // handle whatever happens if locking the parent ptr doesnt work
        }
    }

}

// Serialize thingy from metadata
Thingy::Thingy(Metadata *m) {
    : Thingy("") {

    std::ifstream file(metadata->filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error: Failed to open file for reading." << std::endl;
    }
    
    
    file.read(reinterpret_cast<char*>(this),
              sizeof(*this));
    file.close();
    std::cout << "Object deserialized successfully." << std::endl;
}

Thingy& Thingy::createChild(std::string n) {
    auto child = std::make_shared<Thingy>(n);
    addChild(child);
    return *child;
}

void Thingy::addChild(std::shared_ptr<Thingy> child) {
    children.push_back(child);
    
    // if our thing doesn't have a shared ptr to it, shared_from_this will fail
    if (!weak_from_this().expired()) {
        child->parent = shared_from_this();
    } else {
        //handle it differently idk
    }
}
        
void Thingy::removeChild(Thingy* thingy) {
    auto it = std::find_if(children.begin(), children.end(),
        [thingy](const std::shared_ptr<Thingy>& child) {
            return child.get() == thingy;
        });
    
    if (it != children.end()) {
        children.erase(it);  // This will destroy the shared_ptr
    }
}

void Thingy::setParent(std::shared_ptr<Thingy> newParent) {
    newParent->addChild(shared_from_this());
}

void Thingy::setName(std::string n) {
    name = n;
}

std::string Thingy::getName() {
    return(name);
}

void Thingy::serializeHierarchy(Thingy *root, std::string filename) {
        // Metadata metadata;

        // structure of data will look like this: 
        
        // # part 1: thingy hierarchy
        // total amount of thingies << 
        // object ID << object name (length of string, string) << 
        // amount of components << {ID of each component} <<
        // amount of children << {ID of each child} << 
        // object ID << ...
    
        // # part 2: component data
        // total amount of components <<
        // component ID << component data <<
        // component ID << ...
        
        
        // 1: Build object-ID map for thingies and components
        std::map<Thingy*, unsigned> thingyIDMap;
        std::map<Component*, unsigned> componentIDMap;

        // Traverse the tree depth-first to add every 'thingy' to an ID map.
        unsigned thingyId = 0;
        unsigned componentId = 0;
        std::stack<Thingy*> stack;
        Thingy* node;
        stack.push(root);
        
        while (!stack.empty()) {
            node = stack.top();
            stack.pop();

            // Skip if already visited
            if (thingyIDMap.count(node)) continue;

            thingyIDMap[node] = thingyId++;
            
            // Push children to stack 
            for (auto& child : node->children) {
                if (child) {
                    Thingy* childRef = child.get();
                    if (!thingyIDMap.count(childRef))
                    stack.push(childRef);
                }
            }

            // Add each component to componentIDMap
            for (auto& component : node->components) {
                componentIDMap[component.get()] = componentId++;
            }
        }

        // 2: Write to metadata
        // see https://isocpp.org/wiki/faq/serialization#serialize-binary-format
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr
                << "Error: Failed to open file for writing."
                << std::endl;
            return;
        }
        
        // FIX: handle if child or component is null (by just writing a zero probably)
        
        // total thingy count
        file << " " << thingyIDMap.size();

        for (const auto& [thingy, id] : thingyIDMap) {
            // thingy ID
            file << 
                " " << id << 
            // thingy name
                " " << thingy->getName().length() << 
                " " << thingy->getName() << 
            // amount of components
                " " << thingy->components.size();
            // component IDs
            for (auto& component : thingy->components)
                file << " " << componentIDMap[component.get()];
            // amount of children
            file << " " << thingy->children.size();
            // child IDs
            for (auto& child : thingy->children)
                file << " " << thingyIDMap[child.get()];
        }

        // total component count 
        file << " " << componentIDMap.size();

        for (const auto& [component, id] : componentIDMap) {
            // component ID
            file << 
                " " << id << 
                " " << component->value.length() << 
                " " << component->value;
            
            component->serialize(file);
        }

        file.close();
        std::cout << "Hierarchy serialized successfully." << std::endl;
        // return metadata;
    }

