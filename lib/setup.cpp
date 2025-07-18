#include "setup.hpp"

#include <iostream>

#include <dlfcn.h>


bool initDove() {

    // load other important modules
    // if (!loadModules()) {
    //     std::cerr << "Failed to load modules" << std::endl;
    //     return false;
    // }
    
    // load module to build test dream
    {
        void* handle = dlopen("modules/libtest.so", RTLD_NOW);
        if (!handle) {
            std::cerr << "Failed to load libtest.so: " << dlerror() << std::endl;
            return false;
        }

        auto buildTestDream = (void (*)()) dlsym(handle, "buildTestDream");
        buildTestDream();
        dlclose(handle);
    }

    return true;
}

bool loadModules() {
    return (
        loadModule("common") && 
        loadModule("3d") && 
        loadModule("panel") && 
        loadModule("window") && 
        loadModule("asset") && 
        loadModule("camera") && 
        loadModule("physics") && 
        loadModule("game") &&
        loadModule("link")
    );
}

bool loadModule(std::string && name) {
    std::cout << "Loading module " << name << "..." << std::endl;
    
    std::string filePath = "modules/lib" + name + ".so";

    void* handle = dlopen(filePath.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (!handle) {
        std::cerr << "Failed to load lib" << name << ".so: " << dlerror() << std::endl;
        return false;
    }

    dlclose(handle);

    std::cout << "Loaded module " << name << ": done" << std::endl;
    return true;

}

