#include "../core/thingy/thingy.hpp"

class File : public Thingy {
CLASS_DECLARATION(File)
public: 
    File(std::string && initialPath = "");

    bool setPath(std::string && newPath);
    bool rename(std::string && newName);
}
