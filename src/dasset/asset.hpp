#ifndef DASSET_HPP
#define DASSET_HPP

#include <memory>
#include <src/components/component.hpp>


class Asset : public Component {
CLASS_DECLARATION(Asset)
public: 
    
    Asset( std::string && initialValue)
        : Component(std::move( initialValue )) {

        assets.push_back(this);
    }
  
    ~Asset() {
        assets.erase(std::remove(assets.begin(), assets.end(), this), assets.end());
    }
    
    static std::shared_ptr<Asset> getAsset(std::string name);

private:

    static inline std::unordered_map<std::string, std::shared_ptr<Asset>> assets;

};

#endif
