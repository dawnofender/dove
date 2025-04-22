#ifndef DMESH_HPP
#define DMESH_HPP
#include <vector>
#include <glm/glm.hpp>
#include <mutex>

class MeshData {
public: 
    MeshData() {}

    std::vector<glm::vec3> getVertices() {
	    std::lock_guard<std::mutex> lock(mv);
        return vertices;
    }

    std::vector<glm::vec3> getColors() {
	    std::lock_guard<std::mutex> lock(mc);
        return colors;
    }
    
    std::vector<glm::vec3> getNormals() {
	    std::lock_guard<std::mutex> lock(mn);
        return normals;
    }

    std::vector<unsigned int> getIndices() {
	    std::lock_guard<std::mutex> lock(mi);
        return indices;
    }


    void clearVertices() {
	    std::lock_guard<std::mutex> lock(mv);
        vertices.clear();
    }

    void clearColors() {
	    std::lock_guard<std::mutex> lock(mc);
        colors.clear();
    }

    void clearNormals() {
	    std::lock_guard<std::mutex> lock(mn);
        normals.clear();
    }

    void clearIndices() {
	    std::lock_guard<std::mutex> lock(mi);
        indices.clear();
    }


    void clear() {
        clearVertices();
        clearColors();
        clearNormals();
        clearIndices();
    }

private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    std::mutex mv;
    std::mutex mc;
    std::mutex mn;
    std::mutex mi;

};

#endif
