#ifndef DMESH_HPP
#define DMESH_HPP
#include <vector>
#include <glm/glm.hpp>
#include <mutex>
#include <lib/threadsafe_vector.hpp>
#include <iostream>

struct MeshData {

    threadsafe_vector<glm::vec3> vertices;
    threadsafe_vector<glm::vec3> colors;
    threadsafe_vector<glm::vec3> normals;
    threadsafe_vector<unsigned int> indices;

    void clear() {
        vertices.clear();
        colors.clear();
        normals.clear();
        indices.clear();
    }

};

#endif
