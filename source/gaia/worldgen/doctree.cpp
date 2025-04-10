#include <memory>
#include <array>
#include <map>
#include <unordered_map>
#include <cstdint>
#include <glm/glm.hpp>

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

inline float getCellSize(uint8_t depth) {
    return sizeLookup[depth + 128];
}

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

    Octree(int8_t mind, int8_t maxd) : minDepth(mind), maxDepth(maxd){}
};

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

OctreeNode* findNeighbor(OctreeNode* node, Direction dir) { //maybe keep track of distance too so we can walk all the way back down?
    if (!node->parent) return nullptr;              // hit the root, no neighbor
    uint8_t idx = node->indexInParent;
    //if (node->neighbors[dir]) return node->neighbors[dir];
    // 1) If the neighbor is just a sibling in the same parent, return it.
    if (hasSiblingInDir(idx, dir)) {
      return node->parent->children[siblingIndex(idx,dir)].get();
    }
    // 2) Otherwise, recurse to find the parent’s neighbor in that dir
    OctreeNode* parentNbr = findNeighbor(node->parent, dir);
    if (!parentNbr) return nullptr;
    // 3) If that neighbor isn’t subdivided, it *is* our neighbor
    if (!parentNbr->children[0])  // assume “no children” means leaf
      return parentNbr;
    // 4) Otherwise, descend into the appropriate child of that subtree
    //    which “mirrors” our index in this axis:
    return parentNbr->children[siblingIndex(idx,dir)].get();
}
