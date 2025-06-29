#ifndef DTHING_HPP
#define DTHING_HPP

#include "rtti_macros.hpp"
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <cstddef>
#include <utility>
#include <iostream>

// Thing is an abstract class that every object in the hierarchy inherits from.
// This way, all components, thingies, assets, etc. can be accessible in-world.
// Each derived class will have a type, hashed as a size_t, that can be used to identify it.
// Later, this class will also make it easier to serialize/deserialize objects, by assigning each thing an ID.

// The base Thing class has just the bare minimum functionality, and is not meant to be interacted with in-world.
// There is no tree structure - derived classes must implement this.

// NOTE: every thing-derived class must be constructable with no passed arguments.
//  - this is for serialization: create all the things, then give them the data.
//    the error thrown if you forget a constructor originates in the way ThingFactory registers new thing types.
//      - this could be made less annoying with some small changes to the declaration macro and ThingFactory class,
//        however, that could lead to issues with serialization down the line.

// TODO: maybe find a more efficient way to accomplish this? 
// when objects are first constructed with no arguments, it takes extra time to assign them.
// also its just kind of annoying to make every thing class work this way

// TODO: 
//  - separate serializability from thing and use multiple inheritance for this instead
//      - 'Archivable' base class

class Archive;

struct new_enable_shared_from_this :
    public std::enable_shared_from_this<new_enable_shared_from_this> {
    
    template <typename Self>
    auto new_shared_from_this(this Self& self) {
        return std::static_pointer_cast<Self>(self.shared_from_this());
    }
};


class Thing : public new_enable_shared_from_this {
protected:    
    static const std::size_t Type;

public: 
    Thing() = default;

    virtual bool IsClassType( const std::size_t classType ) const { 
        return classType == Type;
    }

    virtual std::size_t getType() const { 
        return Type; 
    }

    virtual void serialize(Archive& ar) {}
    virtual void load() {} // called after all things are deserialized
};


class ThingFactory {
public:

    // singleton accessor
    static ThingFactory& instance()
    {
        static ThingFactory inst;
        return inst;
    }

    // Registers a factory function for a given type‐hash.
    // Returns true if inserted; false if type was already registered.
    bool registerType(std::size_t typeHash, std::function<std::unique_ptr<Thing>()> creator, std::string name) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "registered type hash " << typeHash << " : " << name << std::endl;
        auto res = creators_.emplace(typeHash, std::move(creator));
        return res.second;
    }

    // Creates a new Thing* for the given typeHash. Returns nullptr if not found.
    std::unique_ptr<Thing> create(std::size_t typeHash) const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = creators_.find(typeHash);
        if (it == creators_.end()) {
            std::cerr << "ThingFactory error: creator for type not found " << typeHash << std::endl;
            return nullptr;
        }
        return (it->second)();
    }

private:
    ThingFactory() = default;
    ~ThingFactory() = default;
    ThingFactory(const ThingFactory&) = delete;
    ThingFactory& operator=(const ThingFactory&) = delete;

    mutable std::mutex mutex_;
    std::unordered_map<std::size_t, std::function<std::unique_ptr<Thing>()>> creators_;
};





#endif
