#ifndef DOCTREE_HPP
#define DOCTREE_HPP

#include <array>
#include <map>
#include <memory>
#include <unordered_map>
#include <cstdint>
#include <glm/glm.hpp>
#include <core/thing.hpp>
#include <core/node.hpp>


// TODO: 
//  - rewrite stuff to be less recursive
//  - move some functionality out into a voxel class, which will be stored as the octree's value
//      - neighbors, surface, 
//      - things like transparency should be in a derived voxel class that has color and stuff
//  - octree, 64tree, and thingy all have parent and children, so they could all be derived from something (node?)


enum Direction { POS_X = 0, NEG_X, POS_Y, NEG_Y, POS_Z, NEG_Z };

class OctreeNode : public Thing, public Node<OctreeNode, std::array<std::shared_ptr<OctreeNode>, 8>> {
CLASS_DECLARATION(OctreeNode)
public:
    OctreeNode() {}
    
    bool transparent;
    bool surface = false;
    bool leaf; 
    uint8_t indexInParent = 0;  // 0..7 as per (x<<2)|(y<<1)|z
    std::array<std::shared_ptr<OctreeNode>, 6> neighbors = {nullptr}; // ±X, ±Y, ±Z

    static std::shared_ptr<OctreeNode> findNeighbor(std::shared_ptr<OctreeNode> node, Direction dir); 
    

    // // Proxy object for operator[] chaining
    // struct ProxyZ {
    //     Octree& octree;
    //     int x, y;
    //
    //     T& operator[](int z) { return octree.at(x, y, z); }
    // };
};

class OctreeRoot : public OctreeNode {
public:
    OctreeRoot(int8_t mind, int8_t maxd) 
        : minDepth(mind), maxDepth(maxd){}

    int8_t maxDepth;
    int8_t minDepth;
    glm::vec3 origin;

    std::shared_ptr<OctreeNode> root = std::make_shared<OctreeNode>();

    std::unordered_map<std::shared_ptr<OctreeNode>, glm::vec3> positionMap;
    std::unordered_map<std::shared_ptr<OctreeNode>, int8_t> depthMap;
    
    void indexLeaves(std::shared_ptr<OctreeNode>(cell), int8_t cellDepth, glm::vec3 cellPos);
    static float getCellSize(int8_t depth);
    std::pair<std::shared_ptr<OctreeNode>, glm::vec3> locateCell(glm::vec3 targetPos, int8_t targetDepth);
    std::pair<std::shared_ptr<OctreeNode>, glm::vec3> makeCell(glm::vec3 targetPos, int8_t targetDepth);
    void setNeighbors(std::shared_ptr<OctreeNode> node);
};

#endif
