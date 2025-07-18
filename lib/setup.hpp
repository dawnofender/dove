#ifndef SETUP_HPP
#define SETUP_HPP

#include <string>

bool initDove();

// TODO: move into module loader class
bool loadModules();
bool loadModule(std::string && name);


#endif
