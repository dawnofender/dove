#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <set>
#include <unordered_map>
#include <typeindex>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "components/component.hpp"

class Thing {
private: 
    char* name;
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;
    std::weak_ptr<Thing> parent;
    std::vector<std::shared_ptr<Thing>> children;

public:

    template<typename CompType, typename... Args>
    CompType& addComponent(Args&&... args);

    template<typename CompType>
    inline CompType* getComponent();

    Thing& createChild();
    std::vector<std::shared_ptr<Thing>>& getChildren();

    void setName(char* n);
    char* getName();
    
};

