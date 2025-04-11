#include "doctree.hpp"
#include <iostream>
#include <cmath>
// Define the range of exponents

// Function to compute 2^n at compile time
constexpr float pow2(int n) {
    // We can use a loop in a constexpr function in C++14 and later
    double result = 1.0;
    if (n >= 0) {
        for (int i = 0; i < n; ++i) {
            result *= 2.0;
        }
    } else {
        for (int i = 0; i < -n; ++i) {
            result /= 2.0;
        }
    }
    return result;
}

// Build the lookup table as a constexpr array
constexpr std::array<double, 256> buildSizeTable() {
    std::array<double, 256> table = {};
    for (std::size_t i = 0; i < 256; ++i) {
        int exponent = static_cast<int>(i)-128;
        table[i] = pow2(exponent);
    }
    return table;
}

// Declare the lookup table as a compile-time constant.
constexpr auto sizeLookup = buildSizeTable();

double Octree::getCellSize(uint8_t depth) {
    return sizeLookup[depth + 128];
}

std::pair<std::shared_ptr<OctreeNode>, glm::vec3> Octree::locateCell(glm::vec3 targetPos, int8_t targetDepth) {
    std::shared_ptr<OctreeNode> cell = root;
    int8_t cellDepth = maxDepth;
    glm::vec3 nodeCenter = origin;
    bool searching, dx, dy, dz = true;

    while (cellDepth > targetDepth) {
        cellDepth--;
        const float cellSize = getCellSize(cellDepth);
        nodeCenter += glm::vec3(dx, dy, dz) * glm::vec3(cellSize);

        bool dx = targetPos.x > nodeCenter.x;
        bool dy = targetPos.y > nodeCenter.y;
        bool dz = targetPos.z > nodeCenter.z;
        
        const int childInd = (dx << 2) | (dy << 1) | dz;
        if(cell->children[childInd]) {
            cell = cell->children[childInd];
        } else cell = nullptr;
    }
    return(std::make_pair(cell, nodeCenter));
}

std::pair<std::shared_ptr<OctreeNode>, glm::vec3> Octree::makeCell(glm::vec3 targetPos, int8_t targetDepth) {
    std::shared_ptr<OctreeNode> cell = root;
    int8_t cellDepth = maxDepth;
    glm::vec3 nodePos = origin;
    glm::vec3 nodeCenter;
    bool dx = false;
    bool dy = false; 
    bool dz = false;

    while (cellDepth > targetDepth) {
        cellDepth--;
        nodeCenter = nodePos + glm::vec3(getCellSize(cellDepth-1));
        std::cout << "node center: " << nodeCenter.x << ", " << nodeCenter.y << ", " << nodeCenter.z << "\n";

        dx = targetPos.x > nodeCenter.x;
        dy = targetPos.y > nodeCenter.y;
        dz = targetPos.z > nodeCenter.z;
        
        const int childInd = (dx << 2) | (dy << 1) | dz;

        auto& child = cell->children[childInd];
        if (!child) {
            child = std::make_unique<OctreeNode>();
            child->parent = cell;
            child->indexInParent = childInd;
        }
        cell = std::shared_ptr<OctreeNode>(child);
        nodePos += glm::vec3(dx, dy, dz) * glm::vec3(getCellSize(cellDepth));
    }
    return(std::make_pair(cell, nodePos));
}


// given a node index (0–7) and a direction, can we move within the same parent?
bool hasSiblingInDir(uint8_t idx, Direction d) {
    uint8_t x = (idx>>2)&1, y = (idx>>1)&1, z = idx&1;
    switch(d) {
      case POS_X: return x==0;
      case NEG_X: return x==1;
      case POS_Y: return y==0;
      case NEG_Y: return y==1;
      case POS_Z: return z==0;
      case NEG_Z: return z==1;
    }
    return false;
}

// flip the appropriate bit to get the sibling’s child‑index
uint8_t siblingIndex(uint8_t idx, Direction d) {
    switch(d) {
      case POS_X: return idx | 4;
      case NEG_X: return idx & ~4;
      case POS_Y: return idx | 2;
      case NEG_Y: return idx & ~2;
      case POS_Z: return idx | 1;
      case NEG_Z: return idx & ~1;
    }
    return idx;
}

std::shared_ptr<OctreeNode> findNeighbor(std::shared_ptr<OctreeNode> node, Direction dir) { //maybe keep track of distance too so we can walk all the way back down?
    if (!node->parent) return nullptr;              // hit the root, no neighbor
    uint8_t idx = node->indexInParent;
    //if (node->neighbors[dir]) return node->neighbors[dir];
    // 1) If the neighbor is just a sibling in the same parent, return it.
    if (hasSiblingInDir(idx, dir)) {
      return node->parent->children[siblingIndex(idx,dir)];
    }
    // 2) Otherwise, recurse to find the parent’s neighbor in that dir
    std::shared_ptr<OctreeNode> parentNbr = findNeighbor(node->parent, dir);
    if (!parentNbr) return nullptr;
    // 3) If that neighbor isn’t subdivided, it *is* our neighbor
    if (!parentNbr->children[0])  // assume “no children” means leaf
      return parentNbr;
    // 4) Otherwise, descend into the appropriate child of that subtree
    //    which “mirrors” our index in this axis:
    return parentNbr->children[siblingIndex(idx,dir)];
}

void Octree::setNeighbors(std::shared_ptr<OctreeNode> node) {
    // assign neighbor pointers for this cell
    for (int d = 0; d < 6; ++d){
        node->neighbors[d] = findNeighbor(node, static_cast<Direction>(d));
    }
    
    // recurse into children
    for (auto& child : node->children) {
        if (child) setNeighbors(child);
    }
}

void Octree::indexLeaves(std::shared_ptr<OctreeNode>(cell), int8_t cellDepth, glm::vec3 cellPos) {
    float childSize = getCellSize(cellDepth-1);

    if(cell && cell->leaf) {
        positionMap.insert({std::shared_ptr<OctreeNode>(cell), cellPos});
        depthMap.insert({std::shared_ptr<OctreeNode>(cell), cellDepth});
    } else {
        for (int i = 0; i < 8; ++i) {

            float dx = (i >> 2) & 1;
            float dy = (i >> 1) & 1;
            float dz = i & 1;
            
            glm::vec3 childOffset = glm::vec3(dx, dy, dz) * childSize;

            glm::vec3 childPos = cellPos + childOffset;

            if (cell->children[i]) indexLeaves(cell->children[i], cellDepth-1, childPos);
        }
    }
}
