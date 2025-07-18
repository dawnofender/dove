#ifndef RTTI_MACROS_HPP
#define RTTI_MACROS_HPP

#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <cstddef>

// originally borrowed from:
// https://stackoverflow.com/questions/44105058/implementing-component-system-from-unity-in-c
// many changes have been made to support serialization


#define TO_STRING( x ) #x

//****************
// CLASS_DECLARATION
//
// This macro must be included in the declaration of any subclass of Component.
// It declares variables used in type checking.
//****************
#define CLASS_DECLARATION( classname )                                                      \
public:                                                                                     \
    static const std::size_t Type;                                                          \
    virtual bool IsClassType( const std::size_t classType ) const override;                 \
    virtual std::size_t getType() const override;                                           \
    virtual void initOnce() override;                                                       \

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
        std::hash< std::string >()( TO_STRING( childclass ) );                          \
                                                                                        \
    bool childclass::IsClassType( const std::size_t classType ) const {                 \
        if (classType == childclass::Type)                                              \
            return true;                                                                \
        return parentclass::IsClassType(classType);                                     \
    }                                                                                   \
                                                                                        \
    std::size_t childclass::getType() const { return Type; }                            \
                                                                                        \
    void childclass::initOnce() {                                                       \
        if (!initialized) {                                                             \
            init();                                                                     \
            initialized = true;                                                         \
        }                                                                               \
    }                                                                                   \
                                                                                        \
    /* Static registration at load‐time: */                                             \
    namespace {                                                                         \
        static const bool _registered_##childclass =                                    \
            ThingFactory::instance().registerType(                                      \
                childclass::Type,                                                       \
                TO_STRING( childclass ),                                                \
                []() -> std::unique_ptr<Thing> {                                        \
                    return std::make_unique<childclass>();                              \
                }                                                                       \
            );                                                                          \
    }                                                                                   \


#endif
