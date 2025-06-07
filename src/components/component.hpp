#ifndef DCOMPONENT_HPP
#define DCOMPONENT_HPP

#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <cstddef>

// for components i am currently stealing code from:
// https://stackoverflow.com/questions/44105058/implementing-component-system-from-unity-in-c


// Component base class
class Component {
public: 
    std::string value = "uninitialized";
    static const std::size_t Type;

    virtual ~Component() = default;
    
    Component( std::string && initialValue = "")
        : value( initialValue ) { 
    }
    
    virtual void serialize(std::ostream& out) {
        out << value;
    }

    virtual void unserialize(std::istream& in) {
        in >> value;
    }

    virtual bool IsClassType( const std::size_t classType ) const { 
        return classType == Type;
    }

    virtual std::size_t getType() const { 
        return Type; 
    }

private: 
  static inline std::vector<Component *> dynamicComponents;

public: 
    static void updateAll(); // calls update() on every dynamic component
    virtual void update();
    void registerDynamic();
    void unregisterDynamic();

// public: 
// virtual weak ptr to host 
};


class ComponentFactory
{
public:
    // singleton accessor
    static ComponentFactory& instance()
    {
        static ComponentFactory inst;
        return inst;
    }

    // Registers a factory function for a given type‐hash.
    // Returns true if inserted; false if type was already registered.
    bool registerType(std::size_t typeHash, std::function<std::unique_ptr<Component>()> creator)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto res = creators_.emplace(typeHash, std::move(creator));
        return res.second;
    }

    // Creates a new Component* for the given typeHash. Returns nullptr if not found.
    std::unique_ptr<Component> create(std::size_t typeHash) const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = creators_.find(typeHash);
        if (it == creators_.end()) return nullptr;
        return (it->second)();
    }

private:
    ComponentFactory() = default;
    ~ComponentFactory() = default;
    ComponentFactory(const ComponentFactory&) = delete;
    ComponentFactory& operator=(const ComponentFactory&) = delete;

    mutable std::mutex mutex_;
    std::unordered_map<std::size_t, std::function<std::unique_ptr<Component>()>> creators_;
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


// ####################
// # CLASS_DEFINITION #
// ####################
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
            ComponentFactory::instance().registerType(                                  \
                childclass::Type,                                                       \
                []() -> std::unique_ptr<Component> {                                    \
                    return std::make_unique<childclass>();                              \
                }                                                                       \
            );                                                                          \
    }

#endif
