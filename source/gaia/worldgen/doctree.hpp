#ifndef OCTREE_HPP
#define OCTREE_HPP

#include <array>
#include <map>
#include <memory>
#include <unordered_map>
#include <cstdint>
#include <glm/glm.hpp>

enum Direction { POS_X = 0, NEG_X, POS_Y, NEG_Y, POS_Z, NEG_Z };

struct OctreeNode {
    bool leaf;
    OctreeNode* parent = nullptr;        
    uint8_t indexInParent = 0;  // 0..7 as per (x<<2)|(y<<1)|z
    std::array<std::shared_ptr<OctreeNode>, 8> children = {nullptr};
    std::array<std::shared_ptr<OctreeNode>, 6> neighbors = {nullptr}; // ±X, ±Y, ±Z
};

struct Octree {
    int8_t maxDepth;
    int8_t minDepth;
    std::map<int8_t, float> sizes;

    OctreeNode root;
    std::unordered_map<std::shared_ptr<OctreeNode>, glm::vec3> position;
    std::unordered_map<std::shared_ptr<OctreeNode>, int8_t> depth;

    Octree(int8_t mind, int8_t maxd) 
        : minDepth(mind), maxDepth(maxd){}
};

OctreeNode* findNeighbor(OctreeNode* node, Direction dir); 

inline float getCellSize(uint8_t depth);
