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
    bool transparent;
    bool surface = false;
    bool leaf; 
    std::shared_ptr<OctreeNode> parent = {nullptr};        
    uint8_t indexInParent = 0;  // 0..7 as per (x<<2)|(y<<1)|z
    std::array<std::shared_ptr<OctreeNode>, 8> children = {nullptr};
    std::array<std::shared_ptr<OctreeNode>, 6> neighbors = {nullptr}; // ±X, ±Y, ±Z
};

struct Octree {
    int8_t maxDepth;
    int8_t minDepth;
    glm::vec3 origin;

    std::shared_ptr<OctreeNode> root = std::make_shared<OctreeNode>();

    std::unordered_map<std::shared_ptr<OctreeNode>, glm::vec3> positionMap;
    std::unordered_map<std::shared_ptr<OctreeNode>, int8_t> depthMap;

    //std::map< int8_t, std::unordered_map< glm::vec3, std::shared_ptr<OctreeNode> > > leafMap; // (depth, (position, leaf cell))

    Octree(int8_t mind, int8_t maxd) 
        : minDepth(mind), maxDepth(maxd){}
    
    void indexLeaves(std::shared_ptr<OctreeNode>(cell), int8_t cellDepth, glm::vec3 cellPos);
    static double getCellSize(uint8_t depth);
    std::pair<std::shared_ptr<OctreeNode>, glm::vec3> locateCell(glm::vec3 targetPos, int8_t targetDepth);
    std::pair<std::shared_ptr<OctreeNode>, glm::vec3> makeCell(glm::vec3 targetPos, int8_t targetDepth);
    void setNeighbors(std::shared_ptr<OctreeNode> node);
};

OctreeNode* findNeighbor(OctreeNode* node, Direction dir); 


#endif
