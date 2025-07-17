#ifndef DASSET_HPP
#define DASSET_HPP

#include <memory>
#include "../thingy/thing.hpp"
#include "../thingy/archive/archive.hpp"

// assets are objects shared between multiple components
// generally unchanging, but can be edited, eg. 3D modelling

class Asset : public Thing {
CLASS_DECLARATION(Asset)
public: 
    Asset(std::string && initialName = "Unnamed Asset");

    virtual void serialize(Archive& archive) override;
    virtual void init() override;

public:
    std::string name;
    
};

#endif
