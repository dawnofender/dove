#ifndef DMESH_HPP
#define DMESH_HPP
#include <vector>
#include <glm/glm.hpp>
#include <mutex>
#include <lib/threadsafe_vector.hpp>
#include <iostream>
#include <string>

// NOTE: structure of meshes and relationship to layers is similar to thingies and components.
// maybe this could be simplified later

struct MeshLayer {
    static inline unsigned size = 1;
    std::vector<unsigned> data;

    MeshLayer() = default;

    // Copy constructor
    MeshLayer(const BufferData& source) {
        data = source.data;
    }

    // Constructor from type and data vector
    template <typename T>
    MeshLayer(const std::vector<T>& inputData) {
        data.resize(inputData.size() * sizeof(T) / sizeof(GLunsigned));
        std::memcpy(data.data(), inputData.data(), inputData.size() * sizeof(T));
    }
};

struct IndexLayer : public MeshLayer {
    static inline unsigned size = 1;
    std::vector<unsigned> data;

    IndexLayer(const std::vector<unsigned>& inputData) {
        data = std::vector<unsigned>(inputData);
    }
};

struct Vec2Layer : public MeshLayer {
    static inline unsigned size = 2;
    std::vector<glm::vec2> data;

    Vec2Layer(const std::vector<glm::vec2>& inputData) {
        data = std::vector<glm::vec2>(inputData);
    }
};

struct Vec3Layer : public MeshLayer {
    static inline unsigned size = 3;
    std::vector<glm::vec3> data;

    Vec3Layer(const std::vector<glm::vec3>& inputData) {
        data = std::vector<glm::vec3>(inputData);
    }
};

struct Vec4Layer : public MeshLayer {
    static inline unsigned size = 4;
    std::vector<glm::vec4> data;

    Vec4Layer(const std::vector<glm::vec4>& inputData) {
        data = std::vector<glm::vec3>(inputData);
    }
};

struct MeshData {

    std::vector<MeshLayer> layers;
    // layer 0 is usually for vertices 
    std::vector<unsigned> indices;
    
    MeshData() = default;

    MeshData( MeshData& source ) {
        layers = std::vector<BufferData>( source.layers );
        indices = source.indices;
    }

    MeshData( std::vector<BufferData> l = {} ) 
        : layers(l) {};

    void clear() {
        layers.clear();
    }

};

#endif
