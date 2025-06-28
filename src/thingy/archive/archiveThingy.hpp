#ifndef ARCHIVETHING_HPP
#define ARCHIVETHING_HPP

#include "../thing.hpp"
#include <memory>
#include <iostream>
#include <type_traits>
#include <string>

// tracks pointers to 'things' (everything with this base class (thingy, component) should be serializable)


// TODO: for large amounts of data, it may be useful to count up occurances and give the most common pointers smaller IDs.
// i think a lot of file compression works this way

class PointerTracker {
public:
    int registerPointer(void* ptr) {
        // if this pointer has already been registered, return its ID
        if (pointerToId.count(ptr)) return pointerToId[ptr];
        
        //if its a 0 or a nullptr, we don't need to register it 
        if (!ptr) return 0;

        // otherwise, assign it the next ID and return that
        int id = nextId++;
        pointerToId[ptr] = id;
        return id;
    }
    
    template <typename T>
    void addDeserialized(int id, T*& obj) {
        idToPointer[id] = (void*)obj;
    }
    
    template <typename T>
    void addDeserialized(int id, std::shared_ptr<T> obj) {
        idToSharedPointer[id] = std::static_pointer_cast<void>(obj);
        idToPointer[id] = obj.get();
    }

    // INPUTS:  - the ID of a pointer 
    // OUTPUT:  - if ID has been registered, returns the pointer at the given ID 
    //          - if ID has not been registered, returns nullptr
    void* getPointer(int id) {
        // if the pointer isn't in the map, we just return nullptr
        auto it = idToPointer.find(id);
        return it != idToPointer.end() ? it->second : nullptr;
    }

    std::shared_ptr<void> getSharedPointer(int id) {
        // if the pointer isn't in the map, we just return nullptr
        auto it = idToSharedPointer.find(id);
        return it != idToSharedPointer.end() ? it->second : nullptr;
    }

    // INPUT:   - the ID of a pointer 
    // OUTPUT:  - if ID has been registered, returns the pointer at the given ID 
    //          - if ID has not been registered, returns nullptr
    template <typename T>
    int getID(T*& ptr) {
        // if the ptr is 0, null, nullptr, return 0
        if (!ptr) return 0;
        
        // otherwise, try to find it in the map and return its ID
        // if the pointer isn't in the map, return -1
        auto it = pointerToId.find(ptr);
        return it != pointerToId.end() ? it->second : -1;
    }

private:
    int nextId = 1;
    std::unordered_map<void*, int> pointerToId;
    std::unordered_map<int, void*> idToPointer;
    std::unordered_map<int, std::shared_ptr<void>> idToSharedPointer;
};


class Archive : public Thing {
CLASS_DECLARATION(Archive)
public:
    // TODO: use file thingy as source instead of ostream and istream
    Archive() {};
    Archive(std::ostream* os);
    Archive(std::istream* is);

    template<typename T>
    void serialize(T& obj);

    template<typename T>
    void deserialize(T& obj);
    
    

    // operator '<<' is used to save data to the archive
    // operator '>>' is used to load data from the archive
    
    // ############
    // # pointers #
    // ############
    // every time we see a pointer, we can't just save the memory address.
    // instead, we go into to that object and serialize it too. 
    // we keep track of these using PointerTracker, assigning an ID to each object.
    // TODO: 
    //  - addDeserializeditional prevention of infinite loops, if thats a problem idk yet
    //  - serialize/deserialize vectors
    
    // # raw pointer
    // serialize
    template<typename T>
    Archive& operator<<(T*& ptr) {
        int id = pointerTracker.getID(ptr);

        // we only write the data for a pointer's object once, on its first occurance. 
        // getID() returns -1 only when the ptr has not been registered and is not null.
        if (id < 0) {
            id = pointerTracker.registerPointer(ptr);
            serialize(id);
            
            // Serializable classes have a special serialize function, so we must use this
            // anything else is assumed to be a basic data type so we just save that
            if constexpr (std::is_base_of<Thing, T>::value) {
                std::cout << "serializing object of base class: thing" << std::endl;
                // things
                // NOTE: solid chance this is just going to get the type of the base class and we'll need a shared ptr or something
                std::size_t thingType = ptr->getType();
                std::cout << ptr->getType() << std::endl;
                out->write(reinterpret_cast<const char*>(&thingType), sizeof(thingType));
                ptr->serialize(*this);
            } else {
                std::cout << "serializing object of general base class" << std::endl;
                // basic types
                T data = ptr;
                serialize(data);
            }

        } else serialize(id);

        return *this;
    }

    // deserialize
    template<typename T>
    Archive& operator>>(T*& ptr) {
        std::cout << "deserializing raw ptr" << std::endl;
        int id;
        deserialize(id);

        if (id == 0) ptr = nullptr;
        else if (void* existing = pointerTracker.getPointer((int)id)){
            ptr = static_cast<T*>(existing);
        } else {

            // Things can be instantiated from an archive
            // anything else is assumed to be a basic data type, so we just read the data directly
            if constexpr (std::is_base_of<Thing, T>::value) {
                std::cout << "deserializing object of base class: thing" << std::endl;
                std::size_t thingType;
                deserialize(thingType);
                std::cout << thingType << std::endl;
                std::cout << "testb" << std::endl;
                std::unique_ptr<Thing> uniqueBase = ThingFactory::instance().create(thingType); 
                Thing* rawBase = uniqueBase.release();
                std::cout << "testc" << std::endl;
                ptr = dynamic_cast<T*>(rawBase);
                std::cout << "testd" << std::endl;
                
                if (ptr) 
                    ptr->deserialize(*this);
                else std::cerr << "failed to deserialize" << std::endl;
                std::cout << "teste" << std::endl;

                pointerTracker.addDeserialized(id, ptr);
            } else {
                std::cout << "deserializing object of general base class" << std::endl;
                T data; 
                deserialize(data);
                ptr = data;
                pointerTracker.addDeserialized(id, ptr);
            }
        }
        return *this;
    }

    // # unique pointer
    // serialize
    template<typename T>
    Archive& operator<<(std::unique_ptr<T>& ptr) {
        T* rawPtr = ptr.get();
        serialize(rawPtr);
        return *this;
    }

    // deserialize
    template<typename T>
    Archive& operator>>(std::unique_ptr<T>& ptr) {
        T* rawPtr = nullptr;
        deserialize(rawPtr);
        ptr = std::unique_ptr<T>(rawPtr);
        return *this;
    }

    // # shared pointer
    // serialize
    template<typename T>
    Archive& operator<<(std::shared_ptr<T>& ptr) {
        T* rawPtr = ptr.get();
        int id = pointerTracker.getID(rawPtr);

        // we only write the data for a pointer's object once, on its first occurance. 
        // getID() returns -1 only when the ptr has not been registered and is not null.
        if (id < 0) {
            id = pointerTracker.registerPointer(rawPtr);
            serialize(id);
            // Serializable classes have a special serialize function, so we must use this
            // anything else is assumed to be a basic data type so we just save that
            if constexpr (std::is_base_of<Thing, T>::value) {
                std::cout << "serializing object of base class: thing" << std::endl;
                // things
                std::size_t thingType = ptr->getType();
                serialize(thingType);
                ptr->serialize(*this);
            } else {
                std::cout << "serializing object of general base class" << std::endl;
                // basic types
                T data = ptr;
                serialize(data);
            }

        } else serialize(id);
        
        return *this;
    }

    // deserialize
    template<typename T>
    Archive& operator>>(std::shared_ptr<T>& ptr) {
        std::cout << "deserializing shared ptr" << std::endl;
        int id;
        deserialize(id);
        if (id == 0) ptr = nullptr;
        else if (std::shared_ptr<void> existing = pointerTracker.getSharedPointer((int)id)){
            ptr = std::static_pointer_cast<T>(existing);
        } else {

            // Things can be instantiated from an archive
            // anything else is assumed to be a basic data type, so we just read the data directly
            if constexpr (std::is_base_of<Thing, T>::value) {
                std::cout << "deserializing object of base class: thing" << std::endl;
                std::size_t thingType;
                deserialize(thingType);

                std::cout << "testa" << std::endl;
                std::unique_ptr<Thing> basePtr = ThingFactory::instance().create(thingType);
                std::cout << "testb" << std::endl;
                if (!basePtr) {
                    std::cerr << "error deserializing" << std::endl;
                    return *this;
                }
                std::shared_ptr<Thing> baseShared = std::move(basePtr);
                std::cout << "testc" << std::endl;
                ptr = std::dynamic_pointer_cast<T>(baseShared);
                std::cout << "testd" << std::endl;
                ptr->deserialize(*this);

                std::cout << "teste" << std::endl;

                pointerTracker.addDeserialized(id, ptr);
            } else {
                std::cout << "deserializing object of general type" << std::endl;
                T data; 
                deserialize(data);
                ptr = data;
                pointerTracker.addDeserialized(id, ptr);
            }
        }
        return *this;
    }

    // # weak pointer
    // serialize
    template<typename T>
    Archive& operator<<(std::weak_ptr<T>& ptr) {
        // turn it into a shared ptr by locking it, and serialize that
        // if (auto lockedPtr = ptr.lock()) {
        //     T* rawPtr = lockedPtr.get();
        //     serialize(rawPtr);
        // } else {
        //     std::cerr << "archive failed to lock weak_ptr" << std::endl;
        // }

        return *this;
    }

    // deserialize
    template<typename T>
    Archive& operator>>(std::weak_ptr<T>& ptr) {
        // turn it into a shared ptr by locking it, and deserialize that
        // if (auto lockedPtr = ptr.lock())
        //     deserialize(lockedPtr);
        return *this;
    }

    
    // ###############
    // # Basic types #
    // ###############
    
    // # general value (int, float, double, struct of PODs, etc.):
    // serialize
    template<typename T>
    Archive& operator<<(const T& value) {
        static_assert(std::is_trivially_copyable<T>::value,
                      "writeValue requires a trivially copyable type");
        out->write(reinterpret_cast<const char*>(&value), sizeof(T));
        return *this;
    }

    // deserialize
    template<typename T>
    Archive& operator>>(T& value) {
        static_assert(std::is_trivially_copyable<T>::value,
                      "readValue requires a trivially copyable type");
        in->read(reinterpret_cast<char*>(&value), sizeof(T));
        return *this;
    }

    // # string:
    // serialize
    Archive& operator<<(std::string& str) {
        uint64_t len = str.size();
        serialize(len);
        if (len)
            out->write(str.data(), len);
        return *this;
    }

    // deserialize
    Archive& operator>>(std::string& str) {
        uint64_t len = 0;
        deserialize(len);
        str.resize(len);
        if (len)
            in->read(&str[0], len);
        return *this;
    }

    // # vector:
    // serialize
    template<typename T>
    Archive& operator<<(std::vector<T>& vec) {
        uint64_t count = vec.size();
        serialize(count);
        if (count)
            for (auto && item : vec)
                serialize(item);
        return *this;
    }

    // deserialize
    template<typename T>
    Archive& operator>>(std::vector<T>& vec) {
        std::cout << "deserializing vector" << std::endl;
        uint64_t count = 0;
        deserialize(count);
        vec.resize(count);
        if (count)
            for (auto && item : vec)
                deserialize(item);
        return *this;
    }

    // # vector of ptrs:
    // serialize
    template<typename T>
    Archive& operator<<(std::vector<T*>& vec) {
        uint64_t count = vec.size();
        serialize(count);
        if (count)
            for (auto && item : vec)
                serialize(item);
        return *this;
    }

    // deserialize
    template<typename T>
    Archive& operator>>(std::vector<T*>& vec) {
        std::cout << "deserializing vector" << std::endl;
        uint64_t count = 0;
        deserialize(count);
        vec.resize(count);
        if (count)
            for (auto && item : vec)
                deserialize(item);
        return *this;
    }

private:
    std::ostream* out;
    std::istream* in;

    PointerTracker pointerTracker;
};


template<typename T>
void Archive::serialize(T& obj) {
    (*this) << obj;
}

template<typename T>
void Archive::deserialize(T& obj) {
    (*this) >> obj;
}


#endif
