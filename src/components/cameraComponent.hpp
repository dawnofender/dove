#include "component.hpp"

class Camera : public Component {
CLASS_DECLARATION(Camera)
public:
    Camera(std::string &&initialValue = "Camera");
    
    void see();
private: 

};
