#ifndef ARCHIVETHING_HPP
#define ARCHIVETHING_HPP

#include "../thing.hpp"
#include <memory>
#include <any>
#include <iostream>
#include <type_traits>
#include <string>

// tracks pointers to 'things' (everything with this base class (thingy, component) should be serializable)

// NOTE: Archivable 

// TODO: 
//  - we want to make sure that if one object fails, it won't break everything. 
//      - for this, we should be able to skip an object during deserialization.
//      - one way to do this would be to write the serialized size of an object, so we know where to go if it fails.
//  - for large amounts of data, count up occurances and give the most common pointers smaller IDs.
//  - to implement the features above, we will have to do multiple passes to collect data. 
//      1: Archival traces a tree-like path as it traverses through pointers. This tree should be saved
//          - depth first
//          - there will almost definitely be loops
//      2: now using our tree instead of the objects themselves, count up pointer occurances, and sort by most common
//      3: 

// ############
// # pointers #
// ############
// when we see a pointer, we can't just save the memory address.
// we need to assign each one an ID and save the object data.
// object data will be written only once - if we see the pointer again, we'll just write the ID.

class PointerTracker {
public:
    // was having issues with this deleting objects but thats just cause i was using shared ptrs and not weak ptrs cause weak ptrs weren't working but that actually wasn't the weak ptrs fault i think
    // ~PointerTracker () {
    //     std::cout << "pointer tracker destructor called" << std::endl;
    //     std::cout << "clearing raw ptrs..." << std::endl;
    //     idToPointer.clear();
    //     std::cout << "clearing shared ptrs..." << std::endl;
    //     idToSharedPointer.clear();
    //     std::cout << "clearing ids..." << std::endl;
    //     pointerToId.clear();
    // }    

    int registerPointer(void* ptr) {
        // if this pointer has already been registered, return its ID
        if (pointerToId.count(ptr)) return pointerToId[ptr];
        
        // ID 0 is reserved for nullptr, no need to register it
        if (!ptr) return 0;

        // otherwise, assign it the next ID and return that
        int id = nextId++;
        pointerToId[ptr] = id;
        return id;
    }
    
    // register any raw pointer type
    template <typename T>
    int registerPointer(T* ptr) {
        return registerPointer((void*)ptr);
    }

    // register any shared pointer type
    template <typename T>
    int registerPointer(std::shared_ptr<T> ptr) {
        // get raw ptr and register that instead
        return registerPointer((void*)ptr.get());
    }
    
    template <typename T>
    void addDeserialized(int& id, T*& obj) {
        idToPointer[id] = (void*)obj;
    }
    
    // TODO: add check that obj type T is base of Thing
    template <typename T>
    void addDeserialized(int& id, std::shared_ptr<T> &obj) {
        idToSharedPointer[id] = obj;
        idToPointer[id] = (void*)obj.get();
    }

    // OUTPUT:  - replaces the given pointer with the found pointer, returns it
    // INPUTS:  - the ID of a pointer to look for
    //          - a reference to a pointer
    template <typename T>
    T*& getPointer(int& id, T*& ptr) {
        
        // find in raw pointer map
        auto it = idToPointer.find(id);
        if (it != idToPointer.end())
            return (ptr = it->second);

        // if the pointer isn't in the map, we can look for it in the shared pointer map
        if constexpr (std::is_base_of<Thing, T>::value) {
            auto it = idToSharedPointer.find(id);
            if (it != idToSharedPointer.end()) {
                if (auto locked = it->second.lock()) {
                    ptr = locked;
                    // found it, add it to the map before returning
                    idToPointer[id] = ptr;
                    return ptr;
                } else 
                    std::cerr << "ERROR: PointerTracker failed to lock weak pointer" << std::endl;
            }
        }

        return (ptr = nullptr);
    }
    
    // same but with shared_ptr 
    template <typename T>
    std::shared_ptr<T>& getPointer(int& id, std::shared_ptr<T> &ptr) {

        //find in shared pointer map
        auto it = idToSharedPointer.find(id);
        if (it != idToSharedPointer.end()) {
            if (auto locked = it->second.lock())
                return (ptr = locked);
            else
                std::cerr << "ERROR: PointerTracker failed to lock weak pointer" << std::endl;
        }
        
        // if the pointer isn't in the map, we can look for it in the raw pointer map
        auto it2 = idToPointer.find(id);
        if (it2 != idToPointer.end()) {
            // found it, add it to the map before returning
            ptr = std::shared_ptr<T>(static_cast<T*>(it2->second));
            idToSharedPointer[id] = ptr;
            return ptr;
        }

        // couldn't find it, return nullptr
        return (ptr = nullptr);

        // if (it != idToSharedPointer.end()) {
        //     if (auto locked = it->second.lock())
        //         return (ptr = locked);
        //     else
        //         std::cout << "ERROR: PointerTracker failed to lock weak pointer";
        // }
        // // if lock failed, or the pointer isn't in the map, we can look for it in the raw ptr map
        // // (nullptr if it isn't found)
        // void* rawPtr;
        // getPointer(id, rawPtr);
        // return (ptr = std::shared_ptr<T>(static_cast<T*>(rawPtr)));
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
        auto it = pointerToId.find((void*)ptr);
        return it != pointerToId.end() ? it->second : -1;
    }

    template <typename T>
    int getID(std::shared_ptr<T>& ptr) {
        // get raw ptr and look for that instead
        T* rawPtr = ptr.get();
        return getID(rawPtr);
    }

private:
    int nextId = 1;
    std::unordered_map<void*, int> pointerToId;
    std::unordered_map<int, void*> idToPointer;
    std::unordered_map<int, std::weak_ptr<Thing>> idToSharedPointer;
};


class Archive;

class Archivist {
public: 
    
    void setHost(Archive &newHost) {
        hostArchive = &newHost;
    }


    template<typename T>
    void serializeArchivable(T& obj);
    
    Archive *hostArchive;
};


class ArchiveWriter : public Archivist {
public:
    
    std::ostream *out;
    PointerTracker pointerTracker;

    ArchiveWriter(std::ostream *os); 

    // # raw pointer
    template<typename T>
    void archive(T*& ptr) {
        int id = pointerTracker.getID(ptr);

        // we only write the data for a pointer's object once, on its first occurance. 
        // getID() returns -1 only when the ptr has not been registered and is not null.
        if (id < 0) {
            id = pointerTracker.registerPointer(ptr);
            archive(id);
            
            // Serializable classes have a special archive function, so we must use this
            // anything else is assumed to be a basic data type so we just save that
            if constexpr (std::is_base_of<Thing, T>::value) {
                // things
                std::size_t typeHash = ptr->getType();
                archive(typeHash);
                serializeArchivable(ptr);
            } else {
                // basic types
                T data = ptr;
                archive(data);
            }

        } else {
            // the data this points to has already been archived, just write the ID
            // nullptrs should always have an ID of 0
            archive(id);
        }
    }


    // # unique pointer
    template<typename T>
    void archive(std::unique_ptr<T>& ptr) {
        auto* rawPtr = ptr.get();

        int id = pointerTracker.getID(rawPtr);

        // we only write the data for a pointer's object once, on its first occurance. 
        // getID() returns -1 only when the ptr has not been registered and is not null.
        if (id < 0) {
            id = pointerTracker.registerPointer(rawPtr);
            archive(id);
            
            // Serializable classes have a special archive function, so we must use this
            // anything else is assumed to be a basic data type so we just save that
            if constexpr (std::is_base_of<Thing, T>::value) {
                // things
                std::size_t typeHash = ptr->getType();
                archive(typeHash);
                serializeArchivable(ptr);
            } else {
                // basic types
                T data = ptr;
                archive(data);
            }

        } else {
            // the data this points to has already been archived, just write the ID
            // nullptrs should always have an ID of 0
            archive(id);
        }
    }

    // # shared pointer
    template<typename T>
    void archive(std::shared_ptr<T>& ptr) {
        int id = pointerTracker.getID(ptr);

        // we only write the data for a pointer's object once, on its first occurance. 
        // getID() returns -1 only when the ptr has not been registered and is not null.
        if (id < 0) {
            id = pointerTracker.registerPointer(ptr);
            archive(id);

            // Archivable classes have a special archive function, so we must use this
            // anything else is assumed to be a basic data type so we just save that
            if constexpr (std::is_base_of<Thing, T>::value) {
                // things
                std::size_t thingType = ptr->getType();
                archive(thingType);
                serializeArchivable(ptr);
            } else {
                // basic types
                T data = ptr;
                archive(data);
            }
        } else {
            // the data this points to has already been archived, just write the ID
            // nullptrs should always have an ID of 0
            archive(id);
        }
    }

    // # weak pointer
    template<typename T>
    void archive(std::weak_ptr<T>& ptr) {
        // turn it into a shared ptr by locking it, and archive that
        if (auto lockedPtr = ptr.lock()) {
            archive(lockedPtr);
        } else {
            std::cerr << "Serialization error: Failed to lock weak_ptr, writing nullptr" << std::endl;
            std::shared_ptr<T> placeholder = nullptr;
            archive(placeholder);
        }
    }

    // ###############
    // # Basic types #
    // ###############
    
    // # general value (int, float, double, struct of PODs, etc.):
    template<typename T>
    void archive(const T& value) {
        static_assert(std::is_trivially_copyable<T>::value,
                      "writeValue requires a trivially copyable type");
        out->write(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    // # string:
    void archive(std::string& str) {
        uint64_t len = str.size();
        archive(len);
        if (len)
            out->write(str.data(), len);
    }

    // # vector:
    template<typename T>
    void archive(std::vector<T>& vec) {
        uint64_t count = vec.size();
        archive(count);
        std::cout << "writing vector, count: " << count << std::endl;
        for (auto && item : vec)
            archive(item);
    }

    // pair:
    template<typename T1, typename T2>
    void archive(std::pair<T1, T2>& p) {
        archive(p.first);
        archive(p.second);
    }

};

class ArchiveReader : public Archivist {
public:

    std::istream *in;
    PointerTracker pointerTracker;

    ArchiveReader(std::istream *is);
        
    // # raw pointer
    template<typename T>
    void archive(T*& ptr) {
        int id;
        archive(id);
        
        // id 0? no data, just nullptr
        if (id == 0) {
            ptr = nullptr;
        } else {
            
            // let's check if we've deserialized this already
            void* existing;
            if (pointerTracker.getPointer(id, existing)) {
                // yep
                std::cout << "    ID " << id << ": retrieving" << std::endl;
                ptr = static_cast<T*>(existing);
                return;
            }
                std::cout << "    ID " << id << ": deserializing" << std::endl;

            // haven't returned yet? new pointer! let's deserialize the data
            // Thing-based objects have their own deserialize function:
            if constexpr (std::is_base_of<Thing, T>::value) {
                std::size_t thingType;
                archive(thingType);
                
                // if our pointer already points to an object of the right type, we won't have to create a new object
                // NOTE: we are assuming that, if this behavior isn't wanted, the user will instead deserialize into an empty object
                if (!ptr || ptr->getType() != thingType) {
                    // create new object of the correct type
                    std::unique_ptr<Thing> uniqueBase = ThingFactory::instance().create(thingType); 
                    if (!uniqueBase) {
                        std::cerr << 
                            "Deserialization error: Raw pointer ThingFactory instantiation failed" << std::endl <<
                            "    ID:            " << id << std::endl <<
                            "    Type hash:     " << thingType << std::endl;
                        ptr = nullptr;
                        return;
                    }
                    // redirect ptr to new object
                    Thing* rawBase = uniqueBase.release();
                    ptr = dynamic_cast<T*>(rawBase);
                }

                // we must track deserialized pointers before calling the object's deserialize function.
                // this is because the object may contain a pointer back to itself, either directly or through encapsulation.
                //  - (eg. thingy has children that contain a weak ptr back to the parent. parent->child->parent)
                // If we run into this, we need to know we've already seen this pointer.
                pointerTracker.addDeserialized(id, ptr);
                deserializedThings.push_back(ptr);
                serializeArchivable(ptr);

            } else {
                // anything else is assumed to be a basic data type, so we just read the data directly:
                T data; 
                archive(data);
                ptr = data;
                pointerTracker.addDeserialized(id, ptr);
            }
        }
    }


    // # unique pointer
    template<typename T>
    void archive(std::unique_ptr<T>& ptr) {
        int id;
        archive(id);
        std::cout << "    ID: " << id << std::endl;
        
        // id 0? no data, just nullptr
        if (id == 0) {
            ptr = nullptr;
        } else {
            
            // let's check if we've deserialized this already
            void* existing;
            if (pointerTracker.getPointer(id, existing)) {
                std::cout << "    ID " << id << ": retrieving" << std::endl;
                // yep
                ptr = std::unique_ptr<T>(static_cast<T*>(existing));
                return;
            }
                std::cout << "    ID " << id << ": deserializing" << std::endl;

            // haven't returned yet? new pointer! let's deserialize the data
            // Thing-based objects have their own deserialize function:
            if constexpr (std::is_base_of<Thing, T>::value) {
                std::size_t thingType;
                archive(thingType);
                
                // if our pointer already points to an object of the right type, we won't have to create a new object
                // NOTE: we are assuming that, if this behavior isn't wanted, the user will instead deserialize into an empty object
                // if (!ptr || ptr->getType() != thingType) {
                    // create new object of the correct type
                    std::unique_ptr<Thing> uniqueBase = ThingFactory::instance().create(thingType); 
                    if (!uniqueBase) {
                        std::cerr << 
                            "Deserialization error: Raw pointer ThingFactory instantiation failed" << std::endl <<
                            "    ID:            " << id << std::endl <<
                            "    Type hash:     " << thingType << std::endl;
                        ptr = nullptr;
                        return;
                    }
                    // redirect ptr to new object
                // }
                ptr = std::unique_ptr<T>(static_cast<T*>(uniqueBase.release()));
                T* rawPtr = ptr.get();

                // we must track deserialized pointers before calling the object's deserialize function.
                // this is because the object may contain a pointer back to itself, either directly or through encapsulation.
                //  - (eg. thingy has children that contain a weak ptr back to the parent. parent->child->parent)
                // If we run into this, we need to know we've already seen this pointer.
                pointerTracker.addDeserialized(id, rawPtr);
                deserializedThings.push_back(rawPtr);
                serializeArchivable(ptr);

            } else {
                // anything else is assumed to be a basic data type, so we just read the data directly:
                T data; 
                archive(data);
                ptr = data;
                T* rawPtr = ptr.get();
                pointerTracker.addDeserialized(id, rawPtr);
            }
        }
    }

    // # shared pointer
    template<typename T>
    void archive(std::shared_ptr<T>& ptr) {
        int id;
        archive(id);

        // id 0? no data, just nullptr
        if (id == 0) {
            ptr = nullptr;
        } else {
            
            // let's check if we've deserialized this already
            std::shared_ptr<Thing> existing;
            if (pointerTracker.getPointer(id, existing)) {
                std::cout << "    ID " << id << ": retrieving" << std::endl;
                // yep
                ptr = std::dynamic_pointer_cast<T>(existing);
                return;
            }
                std::cout << "    ID " << id << ": deserializing" << std::endl;

            // haven't returned yet? new pointer! let's deserialize the data
            // Thing-based objects have their own deserialize function:
            if constexpr (std::is_base_of<Thing, T>::value) {
                std::size_t thingType;
                archive(thingType);

                // if our pointer already points to an object of the right type, we won't have to create a new object
                // NOTE: we are assuming that, if this behavior isn't wanted, the user will instead deserialize into an empty object
                if (!ptr || ptr->getType() != thingType) {
                    // create new object of the correct type
                    std::unique_ptr<Thing> basePtr = ThingFactory::instance().create(thingType);
                    if (!basePtr) {
                        std::cerr << 
                            "Deserialization error: Shared pointer ThingFactory instantiation failed" << std::endl <<
                            "    ID:            " << id << std::endl <<
                            "    Type hash:     " << thingType << std::endl;
                        ptr = nullptr;
                        return;
                    }
                    std::shared_ptr<Thing> baseShared = std::move(basePtr);
                    ptr = std::dynamic_pointer_cast<T>(baseShared);
                }

                // we must track deserialized pointers before calling the object's deserialize function.
                // this is because the object may contain a pointer back to itself, either directly or through encapsulation.
                //  - (eg. thingy has children that contain a weak ptr back to the parent. parent->child->parent)
                // If we run into this, we need to know we've already seen this pointer.
                pointerTracker.addDeserialized(id, ptr);
                deserializedThings.push_back(ptr.get());
                serializeArchivable(ptr);


            } else {
                // anything else is assumed to be a basic data type, so we just read the data directly
                T data; 
                archive(data);
                ptr = data;
                pointerTracker.addDeserialized(id, ptr);
            }
        }
    }

    // # weak pointer
    template<typename T>
    void archive(std::weak_ptr<T>& ptr) {
        // deserialize shared instead and make a weak ptr to that
        std::shared_ptr<T> temp = nullptr; 
        archive(temp);
        if (temp) {
            ptr = temp;
        }
        // else nullptr, leave weak_ptr unassigned
    }

    
    // ###############
    // # Basic types #
    // ###############
    
    // # general value (int, float, double, struct of PODs, etc.):
    template<typename T>
    void archive(T& value) {
        static_assert(std::is_trivially_copyable<T>::value,
                      "readValue requires a trivially copyable type");
        in->read(reinterpret_cast<char*>(&value), sizeof(T));
    }

    // # string:
    void archive(std::string& str) {
        uint64_t len = 0;
        archive(len);
        str.resize(len);
        if (len)
            in->read(&str[0], len);
    }

    // # vector:
    template<typename T>
    void archive(std::vector<T>& vec) {
        uint64_t count;
        archive(count);
        vec.resize(count);
        std::cout << "reading vector, count: " << count << std::endl;
        for (auto && item : vec)
            archive(item);
    }

    // pair:
    template<typename T1, typename T2>
    void archive(std::pair<T1, T2>& p) {
        archive(p.first);
        archive(p.second);
    }

    void initializeDeserialized() {
        for (auto& thing : deserializedThings) {
            // NOTE: init() is not necessary for all archivable objects, maybe separate into another base class or just keep in thing class?
            thing->initOnce();
        }
    }

private: 
    std::vector<Thing*> deserializedThings;

};

class Archive {
public:
    // TODO: use file thingy as source instead of ostream and istream
    Archive(std::ostream* os);
    Archive(std::istream* is);
    
    enum Mode { SAVE, LOAD };
    Mode mode;

    void setMode(Mode m) {
        mode = m;
        switch (mode) {
        case Mode::SAVE:
            archiveWriter = std::make_unique<ArchiveWriter>(out);
            archiveWriter->setHost(*this);
        case Mode::LOAD:
            archiveReader = std::make_unique<ArchiveReader>(in);
            archiveReader->setHost(*this);
        }
    }

    template<typename T>
    void serialize(std::shared_ptr<T>& obj);

    template<typename T>
    void deserialize(std::shared_ptr<T>& obj);
    
    // operator '&' is used to save or load data to the archive, depending on what kind of archivist it has.
    // "archive" is used interchangably for both serialization and deserialization.

    // TODO: 
    //  - additional prevention of infinite loops, if that's a problem, idk yet
    //  - raw pointer and shared ptr have a lot of duplicate code. maybe find a way to clean this up sometime
    

    
    
    // yeah so i spent an entire day trying to figure out a way to get all those template functions to work in an archivist derived class that i can switch within this archive class and i couldn't figure it out so no we're doing a switch case hahahahhahhhhhh
    // maybe try using a lambda function someday or something idk
    template<typename T>
    Archive& operator&(T& obj) {
        switch (mode) {
        case Mode::SAVE:
            depth++;
            archiveWriter->archive(obj);
            depth--;
            break;
        case Mode::LOAD:
            depth++;
            archiveReader->archive(obj);
            depth--;
            break;
        }
        
        return *this;
    }

    template<typename T>
    void serializeArchivable(T& obj) {
        obj->serialize(*this);
    }

private:
    int depth = 0; // deserialization recursion depth: currently unused
    std::unique_ptr<ArchiveWriter> archiveWriter;
    std::unique_ptr<ArchiveReader> archiveReader;
    std::istream* in;
    std::ostream* out;
};

template<typename T>
void Archive::serialize(std::shared_ptr<T>& obj) {
    (*this) & obj;
}

template<typename T>
void Archive::deserialize(std::shared_ptr<T>& obj) {
    (*this) & obj;
    archiveReader->initializeDeserialized();
}

template<typename T>
void Archivist::serializeArchivable(T& obj) {
    hostArchive->serializeArchivable(obj);
}

#endif
