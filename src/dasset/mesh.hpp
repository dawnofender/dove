#ifndef DMESH_HPP
#define DMESH_HPP
#include <vector>
#include <glm/glm.hpp>
#include <mutex>
#include <lib/threadsafe_vector.hpp>
#include <iostream>
#include <string>
#include <cstring>
#include <memory>
#include "asset.hpp"

struct MeshLayer;

class Mesh : public Asset {
CLASS_DECLARATION(Mesh)
public: 
    Mesh(std::string && initialName = "Mesh");
    
    virtual void serialize(Archive& archive) override;

    std::vector<std::unique_ptr<MeshLayer>> layers;
    // NOTE: currently, layers have to be in a specifc order for shaders to know what to do with them.
    // layer 0: vertex positions
    // layer 1: vertex colors
    // layer 2: UVs
    // layer 3: normals
    
    std::vector<unsigned> indices;
    

    // Mesh( MeshData& source ) {
    //     layers = source.layers;
    //     indices = source.indices;
    // }
    template<class LayerType, typename... Args>
    LayerType& addLayer(Args&&... args);

    void clear() {
        layers.clear();
    }

};

template< class LayerType, typename... Args >
LayerType& Mesh::addLayer(Args&&... args) {
    auto layer = std::make_unique<LayerType>(std::forward<Args>(args)...);
    LayerType& ref = *layer;
    layers.emplace_back(std::move(layer));
    return ref;
}


// NOTE: structure of meshes and relationship to layers is similar to thingies and components.
// maybe this could be simplified later

class MeshLayer : public Thing{
CLASS_DECLARATION(MeshLayer)
public:
    static const unsigned elementSize;
    std::vector<unsigned> data;
    
    MeshLayer() {}

    // Copy constructor
    MeshLayer(const MeshLayer& source) {
        data = source.data;
    }

    virtual void serialize(Archive& archive) override {
        archive & data;
    }

    // Constructor from type and data vector
    template <typename T>
    MeshLayer(const std::vector<T>& inputData) {
        data.resize(inputData.size() * sizeof(T) / sizeof(unsigned));
        std::memcpy(data.data(), inputData.data(), inputData.size() * sizeof(T));
    }

    virtual std::size_t getSize() {
        return sizeof(unsigned) * data.size();
    }
    
    virtual void* getData() {
        return &data[0];
    }

    virtual unsigned getElementSize() const {
        return 0;
    }
};

class IndexLayer : public MeshLayer {
CLASS_DECLARATION(IndexLayer)
public:
    IndexLayer() {}

    IndexLayer(const std::vector<unsigned>& inputData) {
        data = std::vector<unsigned>(inputData);
    }

    virtual void serialize(Archive& archive) override {
        archive & data;
    }

    virtual std::size_t getSize() override {
        return sizeof(unsigned) * data.size();
    }

    virtual void* getData() override {
        return &data[0];
    }

    virtual unsigned getElementSize() const override {
        return elementSize;
    }

    static const unsigned elementSize = 1;
    std::vector<unsigned> data;
};

class Vec2Layer : public MeshLayer {
CLASS_DECLARATION(Vec2Layer)
public:
    Vec2Layer() {}

    Vec2Layer(const std::vector<glm::vec2>& inputData) {
        data = std::vector<glm::vec2>(inputData);
    }

    virtual void serialize(Archive& archive) override {
        archive & data;
    }

    virtual std::size_t getSize() override {
        return sizeof(glm::vec2) * data.size();
    }

    virtual void* getData() override {
        return &data[0];
    }

    virtual unsigned getElementSize() const override {
        return elementSize;
    }
    
    static const unsigned elementSize = 2;
    std::vector<glm::vec2> data;
};

class Vec3Layer : public MeshLayer {
CLASS_DECLARATION(Vec3Layer)
public:
    Vec3Layer() {}

    Vec3Layer(const std::vector<glm::vec3>& inputData) {
        data = std::vector<glm::vec3>(inputData);
    }

    virtual void serialize(Archive& archive) override {
        archive & data;
    }

    virtual std::size_t getSize() override {
        return sizeof(glm::vec3) * data.size();
    }

    virtual void* getData() override {
        return &data[0];
    }

    virtual unsigned getElementSize() const override {
        return elementSize;
    }

    static const unsigned elementSize = 3;
    std::vector<glm::vec3> data;
};

class Vec4Layer : public MeshLayer {
CLASS_DECLARATION(Vec4Layer)
public:
    Vec4Layer() {}

    Vec4Layer(const std::vector<glm::vec4>& inputData) {
        data = std::vector<glm::vec4>(inputData);
    }

    virtual void serialize(Archive& archive) override {
        archive & data;
    }

    virtual std::size_t getSize() override {
        return sizeof(glm::vec4) * data.size();
    }
    
    virtual unsigned getElementSize() const override {
        return elementSize;
    }

    static const unsigned elementSize = 4;
    std::vector<glm::vec4> data;
};


#endif
