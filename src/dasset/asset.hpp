#ifndef DASSET_HPP
#define DASSET_HPP

#include <memory>
#include "../thingy/thing.hpp"
#include "../thingy/archive/archiveThingy.hpp"


class Asset : public Thing {
CLASS_DECLARATION(Asset)
public: 
    Asset(std::string && initialName = "Asset");

    virtual void serialize(Archive& archive) override;
    virtual void init() override;

public:
    std::string name;
    
};

#endif
