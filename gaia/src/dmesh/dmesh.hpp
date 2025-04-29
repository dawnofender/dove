#ifndef DMESH_HPP
#define DMESH_HPP
#include <vector>
#include <glm/glm.hpp>
#include <mutex>
#include <lib/threadsafe_vector.hpp>
#include <iostream>

struct MeshData {

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    
    MeshData() {};

    MeshData (MeshData& meshData) {
        vertices = std::vector<glm::vec3>(meshData.vertices);
        colors =   std::vector<glm::vec3>(meshData.colors);
        normals =  std::vector<glm::vec3>(meshData.normals);
        indices =  std::vector<unsigned int>(meshData.indices);
    }

    void clear() {
        vertices.clear();
        colors.clear();
        normals.clear();
        indices.clear();
    }

};

#endif
