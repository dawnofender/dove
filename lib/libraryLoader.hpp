#ifndef DVLIBRARYLOADER_HPP
#define DVLIBRARYLOADER_HPP

// #if defined(_WIN32)
//   #include <windows.h>
//   using LibHandle = HMODULE;
// #else
  #include <dlfcn.h>
  using LibHandle = void*;
// #endif

#include <string>
#include <iostream>


// TODO: windows support



class LibraryLoader {
public:
    LibraryLoader(const std::string & libraryName);
    LibraryLoader();
    ~LibraryLoader();

    void load(const std::string & libraryName);
    void unload();

    template<typename T>
    T getSymbol(const std::string& symbolName);

private: 
    LibHandle handle;
};



template<typename T>
T LibraryLoader::getSymbol(const std::string& symbolName) {
#if defined(_WIN32)
    // FARPROC symbol = GetProcAddress(handle, symbolName.c_str());
#else
    void* symbol = dlsym(handle, symbolName.c_str());
#endif
    if (!symbol) {
        throw std::runtime_error("Failed to load symbol: " + symbolName);
    }
    return reinterpret_cast<T>(symbol);
}



#endif
