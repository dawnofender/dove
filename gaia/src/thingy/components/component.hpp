#ifndef DCOMPONENT_HPP
#define DCOMPONENT_HPP

#include <memory>
#include <string>


// for components i am currently stealing code from:
// https://stackoverflow.com/questions/44105058/implementing-component-system-from-unity-in-c

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


//****************
// CLASS_DEFINITION
// 
// This macro must be included in the class definition to properly initialize 
// variables used in type checking. Take special care to ensure that the 
// proper parentclass is indicated or the run-time type information will be
// incorrect. Only works on single-inheritance RTTI.
//****************
#define CLASS_DEFINITION( parentclass, childclass )                                         \
const std::size_t childclass::Type = std::hash< std::string >()( TO_STRING( childclass ) ); \
bool childclass::IsClassType( const std::size_t classType ) const {                         \
        if ( classType == childclass::Type )                                                \
            return true;                                                                    \
        return parentclass::IsClassType( classType );                                       \
}                                                                                           \


// Component base class
class Thingy;
class Component {
public: 
    static const std::size_t Type;
    virtual bool IsClassType( const std::size_t classType ) const { 
        return classType == Type;
    }

public: 
    virtual ~Component() = default;
    Component( std::string && initialValue )
        : value( initialValue ) { 
    }

public:
    std::string value = "uninitialized";
};


#endif
