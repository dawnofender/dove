#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp> 

class Thing;

class Component {
protected: 
    std::shared_ptr<Thing> host = nullptr;

public: 
    Component(Thing* h) : host(h) {}
    virtual ~Component() {}

    Thing* getThing() {return host;}

};


class Thing {
private: 
    std::vector<Component> components;
    std::shared_ptr<Thing> parent;

};

// class Component {
// public: 
//     virtual ~Component() = default;
// };

template <typename CompType>
inline CompType* getComponent();


class TransformComponent : public Component {
public: 
    glm::vec3 position;
}



