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

struct MeshLayer;

struct MeshData {

    std::vector<std::unique_ptr<MeshLayer>> layers;
    // NOTE: currently, layers have to be in a specifc order for shaders to know what to do with them.
    // layer 0: vertex positions
    // layer 1: vertex colors
    // layer 2: UVs
    // layer 3: normals
    
    std::vector<unsigned> indices;
    
    MeshData() = default;

    // MeshData( MeshData& source ) {
    //     layers = source.layers;
    //     indices = source.indices;
    // }
    template<class LayerType, typename... Args>
    LayerType& addComponent(Args&&... args);

    void clear() {
        layers.clear();
    }

};

template< class LayerType, typename... Args >
LayerType& MeshData::addComponent(Args&&... args) {
    auto layer = std::make_unique<LayerType>(std::forward<Args>(args)...);
    LayerType& ref = *layer;
    layers.emplace_back(std::move(layer));
    return ref;
}


// NOTE: structure of meshes and relationship to layers is similar to thingies and components.
// maybe this could be simplified later

class MeshLayer {
public:
    static const unsigned elementSize;
    std::vector<unsigned> data;
    
    MeshLayer() = default;

    // Copy constructor
    MeshLayer(const MeshLayer& source) {
        data = source.data;
    }

    // Constructor from type and data vector
    template <typename T>
    MeshLayer(const std::vector<T>& inputData) {
        data.resize(inputData.size() * sizeof(T) / sizeof(unsigned));
        std::memcpy(data.data(), inputData.data(), inputData.size() * sizeof(T));
    }

    std::size_t getSize() {
        return sizeof(unsigned) * data.size();
    }

    virtual unsigned getElementSize() const {
        return elementSize;
    }
};

class IndexLayer : public MeshLayer {
public:
    static const unsigned elementSize = 1;
    std::vector<unsigned> data;

    IndexLayer(const std::vector<unsigned>& inputData) {
        data = std::vector<unsigned>(inputData);
    }

    virtual unsigned getElementSize() const override {
        return elementSize;
    }
};

class Vec2Layer : public MeshLayer {
public:
    static const unsigned elementSize = 2;
    std::vector<glm::vec2> data;

    Vec2Layer(const std::vector<glm::vec2>& inputData) {
        data = std::vector<glm::vec2>(inputData);
    }

    std::size_t getSize() {
        return sizeof(glm::vec2) * data.size();
    }

    virtual unsigned getElementSize() const override {
        return elementSize;
    }
};

class Vec3Layer : public MeshLayer {
public:
    static const unsigned elementSize = 3;
    std::vector<glm::vec3> data;

    Vec3Layer(const std::vector<glm::vec3>& inputData) {
        data = std::vector<glm::vec3>(inputData);
    }

    std::size_t getSize() {
        return sizeof(glm::vec3) * data.size();
    }

    virtual unsigned getElementSize() const override {
        return elementSize;
    }
};

class Vec4Layer : public MeshLayer {
public:
    static const unsigned elementSize = 4;
    std::vector<glm::vec4> data;

    Vec4Layer(const std::vector<glm::vec4>& inputData) {
        data = std::vector<glm::vec4>(inputData);
    }

    std::size_t getSize() {
        return sizeof(glm::vec4) * data.size();
    }

    virtual unsigned getElementSize() const override {
        return elementSize;
    }
};
#endif
