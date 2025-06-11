#include "thingy.hpp"
#include <algorithm>
#include <vector>

CLASS_DEFINITION(Thing, Thingy)


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

struct SerializedThingy {
    unsigned ID;
    std::string name;
    unsigned componentCount;
    unsigned childCount;
    std::stack<unsigned> componentIDs;
    std::stack<unsigned> childIDs;
};

// Serialize thingy from metadata
// Thingy::Thingy(Metadata *metadata)
//     : Thingy("") {
//
//     std::ifstream file(metadata->filename, std::ios::binary);
//     if (!file.is_open()) {
//         std::cerr << "Error: Failed to open file for reading." << std::endl;
//     }
//     
//     // # metadata contains 2 things: thingy hierarchy info, and component data.
//     // 1) build ID maps
//     
//     // keep track of relationships by mapping thingies to the IDs of their children and components.
//     // thingies -> child IDs
//     // thingies -> component IDs
//     
//     unsigned totalThingies;
//     unsigned totalComponents;
//     file >> totalThingies;
//     file >> totalComponents;
//      
//     // discover thingies and add them to maps
//     std::unordered_map<std::shared_ptr<Thingy>, SerializedThingy> thingyDataMap;
//     std::map<unsigned, std::shared_ptr<Thingy>> IDThingyMap;
//     std::map<unsigned, Component*> IDComponentMap;
//     std::cout << "total thingies to deserialize: " << totalThingies << std::endl;
//     for (int i = 0; i < totalThingies; i++) {
//         SerializedThingy thingyData;
//         
//         // read values from metadata file
//         file >> 
//             thingyData.ID >> 
//             std::quoted(thingyData.name) >>
//             thingyData.componentCount >> 
//             thingyData.childCount;
//         
//
//         // discover components and add them to map
//         for (int i = 0; i < thingyData.componentCount; i++) {
//             std::size_t componentType;
//             thingyData.componentIDs.push(0);
//             
//             // read values from metadata file
//             file >> thingyData.componentIDs.top() >> componentType;
//
//             // there may be many types of components that inherit from the base component class,
//             // so we will make use of ComponentFactory to instantiate components of any type.
//             // NOTE: this could be improved in serialization by hashing the component types at the start of the metadata file.
//             auto component = ComponentFactory::instance().create(componentType).get();
//             if (!ComponentFactory::instance().create(componentType)) {
//                 std::cout << "error creating component type: " << componentType << std::endl;
//             }
//             IDComponentMap[thingyData.componentIDs.top()] = component;
//         }
//         
//         // discover children and add them to map
//         for (int i = 0; i < thingyData.childCount; i++) {
//             thingyData.childIDs.push(0);
//             file >> thingyData.childIDs.top();
//         }
//
//         // create a thingy for each ID, with null relationships for now.
//         std::shared_ptr<Thingy> thingy = std::make_shared<Thingy>(thingyData.name);
//         thingy->children.resize(thingyData.childCount);
//         thingy->components.resize(thingyData.componentCount);
//         
//         thingyDataMap[thingy] = thingyData;
//         IDThingyMap[thingyData.ID] = thingy;
//
//         std::cout << thingyData.name << " : " << thingyData.ID << std::endl <<
//             "| " << thingyData.componentCount << " components"  << std::endl << 
//             "| " << thingyData.childCount     << " children"    << std::endl;
//     }
//     
//     // 2) build hierarchy
//     //    - connect children to parents
//     //    - connect components to host thingy
//     for (auto& [thingy, thingyData] : thingyDataMap) {
//         std::cout << thingyData.name << std::endl;
//         for (int i = thingyData.childCount-1; i >= 0; i--) {
//             auto& child = IDThingyMap[thingyData.childIDs.top()]; thingyData.childIDs.pop();
//             thingy->children[i] = child;
//         }
//
//         for (int i = thingyData.componentCount-1; i >= 0; i--) {
//             auto& component = IDComponentMap[thingyData.componentIDs.top()]; 
//             thingyData.componentIDs.pop();
//             thingy->components[i].reset(component);
//         }
//     }
//     std::cout << "test0" << std::endl;
//     
//     // 3) load component data
//     unsigned id;
//     for (int i = 0; i < totalComponents; i++) {
//         std::cout << i << std::endl;
//         file >> id;
//         std::cout << id << std::endl;
//         auto&& component = IDComponentMap[id];
//         if (!component) continue;
//         std::cout << "test1" << std::endl;
//         component->unserialize(file); //FIX: 
//         std::cout << "test2" << std::endl;
//     }
//     
//     file.close();
//     std::cout << "Object deserialized successfully." << std::endl;
// }

Thingy& Thingy::addChild(std::string n) {
    auto child = std::make_shared<Thingy>(n);
    addChild(child);
    return *child;
}

void Thingy::addChild(std::shared_ptr<Thingy> child) {
    children.push_back(child);
    
    // if our thing doesn't have a shared ptr to it, shared_from_this will fail
    if (!weak_from_this().expired()) {
        child->parent = new_shared_from_this();
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
    newParent->addChild(new_shared_from_this());
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
        
        // total thingy and component count
        file << " " << thingyIDMap.size();
        file << " " << componentIDMap.size();

        for (const auto& [thingy, id] : thingyIDMap) {
            // thingy ID,
            // thingy name,
            // amount of components,
            // amount of children
            file << 
                " " << id << 
                " \"" << thingy->getName() << "\"" <<
                " " << thingy->components.size() <<
                " " << thingy->children.size();
            // component IDs and types,
            // child IDs
            for (auto && component : thingy->components) {
                file << " " << componentIDMap[component.get()] <<
                        " " << component->getType();

            }
            
            for (auto& child : thingy->children)
                file << " " << thingyIDMap[child.get()];
        }


        for (const auto& [component, id] : componentIDMap) {
            // component id 
            file << " " << id << " ";

            // component data
            std::cout << component->value << std::endl;
            if (component && component->value != "uninitialized")
            component->serialize(file);
        }

        file.close();
        std::cout << "Hierarchy serialized successfully." << std::endl;
        // return metadata;
    }

