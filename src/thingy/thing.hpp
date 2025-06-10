
#ifndef DTHING_HPP
#define DTHING_HPP

#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <cstddef>

// The purpose of Thing is to be an abstract class that all interactible objects inherit from.
// Each derived class will have a type, hashed as a size_t, that can be used to identify it.
// This way, all components, thingies, assets, etc. can exist within the same world hierarchy.
// This allows users to interact with thingies and components as tangible objects within their world.

// The base Thing class has just the bare minimum functionality, and is not meant to be interacted with in-world.
// There is no tree structure - derived classes must implement these.

// type system & macros borrowed from:
// https://stackoverflow.com/questions/44105058/implementing-component-system-from-unity-in-c


class Thing {
protected:
    static const std::size_t Type;
public: 

    Thing() {}
    virtual ~Thing() = default;
    Thing(const Thing&) = delete;
    Thing& operator=(const Thing&) = delete;

    virtual bool IsClassType( const std::size_t classType ) const { 
        return classType == Type;
    }

    virtual std::size_t getType() const { 
        return Type; 
    }
};


class ThingFactory
{
public:
    // singleton accessor
    static ThingFactory& instance()
    {
        static ThingFactory inst;
        return inst;
    }

    // Registers a factory function for a given type‐hash.
    // Returns true if inserted; false if type was already registered.
    bool registerType(std::size_t typeHash, std::function<std::unique_ptr<Thing>()> creator)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto res = creators_.emplace(typeHash, std::move(creator));
        return res.second;
    }

    // Creates a new Component* for the given typeHash. Returns nullptr if not found.
    std::unique_ptr<Thing> create(std::size_t typeHash) const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = creators_.find(typeHash);
        if (it == creators_.end()) return nullptr;
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


#define TO_STRING( x ) #x

//****************
// CLASS_DECLARATION
//
// This macro must be included in the declaration of any subclass of Component.
// It declares variables used in type checking.
//****************
#define CLASS_DECLARATION( classname )                                                      \
public:                                                                                     \
    classname() = default;                                                                  \
    static const std::size_t Type;                                                          \
    virtual bool IsClassType( const std::size_t classType ) const override;                 \
    virtual std::size_t getType() const override;                                           \


//****************
// CLASS_DEFINITION
// 
// This macro must be included in the class definition to properly initialize 
// variables used in type checking. Take special care to ensure that the 
// proper parentclass is indicated or the run-time type information will be
// incorrect. Only works on single-inheritance RTTI.
//****************

// Does the original CLASS_DEFINITION, then registers the component type in the factory. 
// This allows for dynamic component initialization, given just a size_t type
// - useful for unserialization while reading component data from file.
#define CLASS_DEFINITION( parentclass, childclass )                                     \
    const std::size_t childclass::Type =                                                \
        std::hash<std::string>()(TO_STRING(childclass));                                \
                                                                                        \
    bool childclass::IsClassType(const std::size_t classType) const {                   \
        if (classType == childclass::Type)                                              \
            return true;                                                                \
        return parentclass::IsClassType(classType);                                     \
    }                                                                                   \
                                                                                        \
    std::size_t childclass::getType() const { return Type; }                            \
                                                                                        \
    /* Static registration at load‐time: */                                             \
    namespace {                                                                         \
        static const bool _registered_##childclass =                                    \
            ThingFactory::instance().registerType(                                  \
                childclass::Type,                                                       \
                []() -> std::unique_ptr<Thing> {                                    \
                    return std::make_unique<childclass>();                              \
                }                                                                       \
            );                                                                          \
    }


#endif
