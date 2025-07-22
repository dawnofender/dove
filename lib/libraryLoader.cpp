#include "libraryLoader.hpp"


LibraryLoader::LibraryLoader(const std::string & name) {
    load(name);
}

LibraryLoader::LibraryLoader() {}

LibraryLoader::~LibraryLoader() {
    unload();
}

void LibraryLoader::load(const std::string & name) {
    std::cout << "Loading module " << name << "..." << std::endl;
    
    std::string filePath = "modules/lib" + name + ".so";

    handle = dlopen(filePath.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (!handle) {
        std::cerr << "Failed to load lib" << name << ".so: " << dlerror() << std::endl;
        return;
    }

    std::cout << "Loaded module " << name << ": done" << std::endl;
}
    
void LibraryLoader::unload() {
    dlclose(handle);
}

// bool initDove() {
//
//     // load other important modules
//     // if (!loadLibrarys()) {
//     //     std::cerr << "Failed to load modules" << std::endl;
//     //     return false;
//     // }
//     
//     // load module to build test dream
//     {
//         void* handle = dlopen("modules/libtest.so", RTLD_NOW);
//         if (!handle) {
//             std::cerr << "Failed to load libtest.so: " << dlerror() << std::endl;
//             return false;
//         }
//
//         auto buildTestDream = (void (*)()) dlsym(handle, "buildTestDream");
//         buildTestDream();
//         dlclose(handle);
//     }
//
//     return true;
// }
