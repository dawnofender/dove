#ifndef GAIACOMPONENT_HPP
#define GAIACOMPONENT_HPP


#include <cmath>
// #include <glm/detail/qualifier.hpp>
#include <vector>
#include <memory>
#include <array>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <queue>
#include <thread>
#include <mutex>
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "component.hpp"
#include <string>
#include <utility>

#include <lib/SimplexNoise.h>
#include <lib/FastNoise.h>
#include <src/dmesh/dmesh.hpp>
#include <src/worldgen/doctree.hpp>
#include <src/thingy/components/meshRendererComponent.hpp>
#include <src/thingy/components/transformComponent.hpp>
#include <src/thingy/thingy.hpp>


// for hashing vec3 (required by threadedcellmap)
namespace std {
  template<>
  struct hash<glm::vec3> {
    std::size_t operator()(glm::vec3 const& v) const noexcept {
      // hash each float member, then combine
      std::size_t h1 = std::hash<float>{}(v.x);
      std::size_t h2 = std::hash<float>{}(v.y);
      std::size_t h3 = std::hash<float>{}(v.z);

      // boost‐style combine:
      std::size_t seed = h1;
      seed ^= h2 + 0x9e3779b9 + (seed<<6) + (seed>>2);
      seed ^= h3 + 0x9e3779b9 + (seed<<6) + (seed>>2);
      return seed;
    }
  };
}


struct CellSampleData {
    std::shared_ptr<OctreeNode> cell;
    float distance;
    glm::vec3 position;
    int8_t depth;
    bool sampleMode; // true to upsample, false to downsample
    //
    CellSampleData(std::shared_ptr<OctreeNode> c, float dist, glm::vec3 p, int8_t d, bool m) 
        : cell(c), distance(dist), position(p), depth(d), sampleMode(m) {}

    friend bool operator< (CellSampleData const& a, CellSampleData const& b) {
        return a.distance < b.distance && a.cell != b.cell;
    }

    friend bool operator== (CellSampleData const& a, CellSampleData const& b) {
        return a.cell == b.cell;
    }

};


class threadedSampleQueue {
public: 
    void push(CellSampleData d) {
		std::lock_guard<std::mutex> lock(m);
        queueData.insert(d);
    }

    void pop() {
		std::lock_guard<std::mutex> lock(m);
        if (!queueData.empty()) {
            queueData.erase(queueData.begin());
        }
    }
    
    bool empty() {
		std::lock_guard<std::mutex> lock(m);
        return queueData.empty();
    }

    CellSampleData top() {
		std::lock_guard<std::mutex> lock(m);
        if (!queueData.empty()) {
            return *queueData.begin();
        }
    }

    int size() {
        std::lock_guard<std::mutex> lock(m);
        return queueData.size();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(m);
        queueData.clear();
    }

private:
    std::set<CellSampleData> queueData;
	std::mutex m;
};


class threadedCellMap {
private: 
    int8_t offset;
    Octree* cellTree;
    std::vector<std::unordered_map<glm::vec3, std::shared_ptr<OctreeNode>>> cellMap_a; // (depth, (position, leaf cell))
    std::unordered_multimap<std::shared_ptr<OctreeNode>, std::pair<int8_t, glm::vec3>> cellMap_b;
    std::mutex m_a;
    std::mutex m_b;

public:

    threadedCellMap(Octree* ot) : cellTree(ot) {
        cellMap_a.resize(cellTree->maxDepth - cellTree->minDepth + 1);
        offset = 0-cellTree->minDepth;
    }

    std::shared_ptr<OctreeNode> find(int8_t depth, glm::vec3 position) {
		std::lock_guard<std::mutex> lock(m_a);
        return cellMap_a[depth + offset].find(position)->second;
    }    
    
    void insert(std::shared_ptr<OctreeNode> cell, int8_t depth, glm::vec3 position) {
		std::lock_guard<std::mutex> lock_a(m_a);
        cellMap_a[depth + offset].insert({position, cell});
		std::lock_guard<std::mutex> lock_b(m_b);
        cellMap_b.insert({cell, {depth, position}});
    }

    std::unordered_map<glm::vec3, std::shared_ptr<OctreeNode>>& get(int8_t depth) {
		std::lock_guard<std::mutex> lock(m_a);
        return cellMap_a[depth + offset];
    }

    std::shared_ptr<OctreeNode> get(int8_t depth, glm::vec3 pos) {
		std::lock_guard<std::mutex> lock(m_a);
        if (cellMap_a[depth + offset].contains(pos)) {
            return cellMap_a[depth + offset].find(pos)->second;
        } else {
            return nullptr;
        }
    }

    std::pair<int8_t, glm::vec3>& get(std::shared_ptr<OctreeNode> cell) {
		std::lock_guard<std::mutex> lock(m_b);
        return cellMap_b.find(cell)->second;
    }

    glm::vec3 getPos(std::shared_ptr<OctreeNode> cell) {
		std::lock_guard<std::mutex> lock(m_b);
        return cellMap_b.find(cell)->second.second;
    }

    std::unordered_multimap<std::shared_ptr<OctreeNode>, std::pair<int8_t, glm::vec3>>& getCellMap() {
		std::lock_guard<std::mutex> lock(m_b);
        return cellMap_b;
    }

    int8_t getDepth(std::shared_ptr<OctreeNode> cell) {
		std::lock_guard<std::mutex> lock(m_b);
        return cellMap_b.find(cell)->second.first;
    }

    std::vector<std::unordered_map<glm::vec3, std::shared_ptr<OctreeNode>>>& get() {
	 	std::lock_guard<std::mutex> lock(m_a);
        return cellMap_a;
    }

    void erase(std::shared_ptr<OctreeNode> cell, int8_t depth, glm::vec3 position) {
		std::lock_guard<std::mutex> lock_a(m_a);
        cellMap_a[depth + offset].erase(position);
		std::lock_guard<std::mutex> lock_b(m_b);
        cellMap_b.erase(cell);
    }

    // erase if we only know the cell
    void erase(std::shared_ptr<OctreeNode> cell) {
		std::lock_guard<std::mutex> lock_b(m_b);
        if (cellMap_b.contains(cell)) {
            auto [depth, position] = cellMap_b.find(cell)->second;
            cellMap_b.erase(cell);
		    std::lock_guard<std::mutex> lock_a(m_a);
            cellMap_a[depth + offset].erase(position);
        }
    }

    int size() {
		std::lock_guard<std::mutex> lock(m_b);
        return cellMap_b.size();
    }

};


class GaiaChunk : public Component {
CLASS_DECLARATION(GaiaChunk)
private: 
    Thingy* host;
    Octree* cellTree;
    std::shared_ptr<OctreeNode> chunk;
    int8_t depth;
    glm::vec3 position;
    MeshRenderer* meshRenderer;
    MeshData mesh;
    MeshData renderedMesh;
    MeshData cube;
    threadedCellMap* leafMap; // (depth, (position, leaf cell))
    
public: 
    GaiaChunk(std::string && initialValue, Thingy* h, Octree* ot, threadedCellMap* lm, std::shared_ptr<OctreeNode> c, int8_t d) 
        : Component(std::move(initialValue)), host(h), cellTree(ot), leafMap(lm), chunk(c), depth(d) {
        
        cube.vertices = {
            // +x    
            glm::vec3(1, 1, 1),
            glm::vec3(1, 0, 1),
            glm::vec3(1, 0, 0),
            glm::vec3(1, 1, 0),            
            // -x    
            glm::vec3(0, 1, 0),                
            glm::vec3(0, 0, 0),                
            glm::vec3(0, 0, 1),                
            glm::vec3(0, 1, 1),                
            // +y    
            glm::vec3(0, 1, 0),
            glm::vec3(0, 1, 1),
            glm::vec3(1, 1, 1),
            glm::vec3(1, 1, 0),            
            // -y    
            glm::vec3(0, 0, 1),                
            glm::vec3(0, 0, 0),                
            glm::vec3(1, 0, 0),                
            glm::vec3(1, 0, 1),                
            // +z    
            glm::vec3(0, 1, 1),                
            glm::vec3(0, 0, 1),                
            glm::vec3(1, 0, 1),                
            glm::vec3(1, 1, 1),                          
            // -z    
            glm::vec3(1, 1, 0),                
            glm::vec3(1, 0, 0),                
            glm::vec3(0, 0, 0),                
            glm::vec3(0, 1, 0),                
        };

        cube.indices = {
            0, 1, 2, 0, 2, 3,       //+x
            4, 5, 6, 4, 6, 7,       //-x
            8, 9, 10, 8, 10, 11,    //+y
            12, 13, 14, 12, 14, 15, //-y
            16, 17, 18, 16, 18, 19, //+z
            20, 21, 22, 20, 22, 23  //-z
        };
        
        std::cout << "test0" << std::endl;
        if (updateMesh()) {
            std::cout << "test1" << std::endl;
            host->addComponent<MeshRenderer>("MeshRenderer", renderedMesh);
            std::cout << "test2" << std::endl;
            meshRenderer = &host->getComponent<MeshRenderer>();
            std::cout << "chunk mesh generated" << std::endl;
        } else {
            std::cout << "tried to generate empty chunk, delete me" << std::endl;
        }
    }

    void updateMeshRenderer() {

    }

    bool updateMesh() {
        mesh.clear();
        updateMesh(chunk, depth+5);
        if (mesh.vertices.size() < 1) return false;

        const float nScale = 64.f;
        const float nLacunarity = 2.f;
        const float nPersistance = 1.f;
        const int nDepth = 5.f;
        const SimplexNoise simplex(0.1f/nScale, 0.5f, nLacunarity, nPersistance);
        
        mesh.colors.resize(mesh.vertices.size());
        mesh.normals.resize(mesh.vertices.size());
        

        // calculate normals
        for (int i = 0; i < mesh.vertices.size(); i+=4) {
            glm::vec3 edge1 = mesh.vertices[i+1] - mesh.vertices[i];
            glm::vec3 edge2 = mesh.vertices[i+2] - mesh.vertices[i];
            glm::vec3 triangleNormal = normalize(cross(edge1, edge2));
            
            mesh.normals[i] = triangleNormal;
            mesh.normals[i+1] = triangleNormal;
            mesh.normals[i+2] = triangleNormal;
            mesh.normals[i+3] = triangleNormal;
        }

        // color mesh
        for (int i = 0; i < mesh.vertices.size(); i++) {
            //mesh->colors[i] = mesh->vertices[i] / size - (1.0/3) + (rand() / double(RAND_MAX));
            //mesh->colors[i] = mesh->vertices[i] / cSize - (1.0/3) + (rand() / double(RAND_MAX));
            const float noiseR = simplex.fractal(nDepth, mesh.vertices[i].x+1000.f, mesh.vertices[i].y, mesh.vertices[i].z);
            const float noiseG = simplex.fractal(nDepth, mesh.vertices[i].x+2000.f, mesh.vertices[i].y, mesh.vertices[i].z);
            const float noiseB = simplex.fractal(nDepth, mesh.vertices[i].x+3000.f, mesh.vertices[i].y, mesh.vertices[i].z);
            mesh.colors[i] = glm::vec3(noiseR + 0.5, noiseG + 0.5, noiseB + 0.5) + glm::vec3(rand() / double(RAND_MAX))/2.f;
            // mesh->colors[i] = mesh->normals[i];
        }
        
        std::cout << "test123" << std::endl;
        if (meshRenderer) {
            std::cout << "test1235" << std::endl;
            meshRenderer->setMesh(std::make_shared<MeshData>(mesh));
            std::cout << "test1236" << std::endl;
        }
        std::cout << "test1234" << std::endl;
        return true;
    }
    
    void updateMesh(std::shared_ptr<OctreeNode> cell, int8_t cellDepth) {

        // recurse into children until we get to the right depth
        if(cellDepth > depth) {
            for (int i = 0; i < 8; ++i) {
                if (cell->children[i]) {
                    updateMesh(cell->children[i], cellDepth-1);
                } 
            }

        } else if(cell->leaf) buildCellMesh(cell);

    }



    void buildCellMesh(std::shared_ptr<OctreeNode> cell) {

       // currently only rendering solid surface blocks - may change
        if(!cell->transparent) {
            for(int i = 0; i < 6; i++) {

                if(!cell->neighbors[i] || cell->neighbors[i]->transparent) {
                    glm::vec3 cellPos = leafMap->getPos(cell);
                    
                    int vertexInd = mesh.vertices.size();
                    int indexInd = mesh.indices.size();
                    int cubeInd = i*4; // which part of the cube mesh to take data from

                    mesh.vertices.resize(vertexInd + 4); // 4 vertices per side
                    mesh.indices.resize(indexInd + 6); // 6 indices per side
                    
                    GLfloat cellSize = Octree::getCellSize(depth);
                    for(int j = 0; j < 4; j++) {
                        mesh.vertices[vertexInd+j] = cellPos + cube.vertices[cubeInd+j] * cellSize;
                    }


                    mesh.indices[indexInd]   = vertexInd;
                    mesh.indices[indexInd+1] = vertexInd+1;
                    mesh.indices[indexInd+2] = vertexInd+2;
                    mesh.indices[indexInd+3] = vertexInd;
                    mesh.indices[indexInd+4] = vertexInd+2;
                    mesh.indices[indexInd+5] = vertexInd+3;
                }
            }
        }
    }
};


class Gaia: public Component {
CLASS_DECLARATION(Gaia)
private: 
    Octree* cellTree;
    float planetSize;
    float lod = 16.f;
    int8_t chunkDepth = 5;
    MeshData worldMesh;
    Thingy* player;
    Thingy* host;
    Transform* playerTransform;

    // std::vector<std::unordered_map<glm::vec3, std::shared_ptr<OctreeNode>>> leafMap; // (depth, (position, leaf cell))
    threadedCellMap* leafMap; // (depth, (position, leaf cell))
    threadedSampleQueue sampleQueue;
    threadedSampleQueue chunkQueue;
    std::unordered_map<std::shared_ptr<OctreeNode>, std::shared_ptr<Thingy>> chunkThingies;

    bool generatingWorld;

    int maxThreads = 4;
    int threadIndex = 0;
    std::vector<std::thread> octreeThreads;

public: 

    Gaia(std::string && initialValue, Thingy* h, Thingy* p)
        : Component(std::move(initialValue)), host(h), player(p) {
        
        // FIX: breaks if transform component doesn't exist
        playerTransform = &player->getComponent<Transform>();
        
    }
    
    void setLocalPlayer(Thingy* p) {
        player = p;
        playerTransform = &player->getComponent<Transform>();
    }

    void createWorld(Octree* ot) {
        std::cout << "gaia: creating world" << std::endl;
        cellTree = ot;
        leafMap = new threadedCellMap(ot);

        planetSize = cellTree->getCellSize(cellTree->maxDepth);
        
        
        float offset = planetSize / -2.f;
        cellTree->origin = glm::vec3(offset);
        //planet.leaf = false;
        //
        double time0;        
        double time1;

        // time0 = glfwGetTime();
        // generateWorld();
        // time1 = glfwGetTime();
        // std::cout << "                     World generated | " << 1000*(time1-time0) << "ms\n";

        // boring method: chunk generation
        // pros:
        //  - easy and fast to make separate meshes from each chunk
        // cons:
        //  - 
        // 1) make octree nodes within a radius (render distance), down to a predifined depth (chunkdepth)
        //  1a) sort cells at chunkdepth by distance 
        // 2) generate closest ungenerated chunks
        // 3) collect data on new chunks: which sides are completely solid
        // 4) generate next closest chunks, excluding ones that won't be visible (using data from 4) (bfs search)
        // .) repeat 4-5 until all chunks from 1a are generated
        
        // or: world resampling
        // 1) make a queue of cells to generate, this will be our first sample
        //  - start with the root node
        // 2) generate matter for all cells in the queue
        // 3) add only surface cells to the queue (next sample)
        //  - exclude cells if their percieved scale is big enough (lod) 
        //  - if player is too far away, unload cell data
        // 4) repeat 2-3 until queue is empty
        // 6) sort cells into chunks for rendering
        
        // probably combine both methds, only doing full chunk generation for chunks that are very close
       
        // setup sample for the root node (entire planet)
        generateMatter(cellTree->root, cellTree->origin + glm::vec3(cellTree->getCellSize(cellTree->maxDepth-1)));
        cellTree->root->leaf = true;
        leafMap->insert(cellTree->root, cellTree->maxDepth, cellTree->origin);
        //sampleQueue.push(CellSampleData(cellTree->root, planetSize, cellTree->origin, cellTree->maxDepth, true));

        std::cout << "gaia: starting sampling" << std::endl;
        // sample enough times to make world around player
        for (int i = 0; i <= cellTree->maxDepth; i++) {
            time0 = glfwGetTime();
            updateSampleData();
            generateSamples();
            time1 = glfwGetTime();
            std::cout << leafMap->size();
            std::cout << "          Generated world sample " << (int) i << " | " << 1000*(time1-time0) << "ms\n";
        }

        time0 = glfwGetTime();
        // simplifyHomogeneous(cellTree->root);
        time1 = glfwGetTime();
        std::cout << "              Simplified homogeneous | " << 1000*(time1-time0) << "ms\n";

        time0 = glfwGetTime();
        // simplifyDuplicates(cellTree->root);
        time1 = glfwGetTime();
        std::cout << "            Simplified doppelgangers | " << 1000*(time1-time0) << "ms\n";

        time0 = glfwGetTime();
        time1 = glfwGetTime();
        std::cout << "             Neighbor data generated | " << 1000*(time1-time0) << "ms\n";

        // time0 = glfwGetTime();
        // buildChunkObjects();
        // time1 = glfwGetTime();
        // std::cout << chunkMap.size();
        // std::cout << "           Chunk mesh data generated | " << 1000*(time1-time0) << "ms\n";
        
        // chunkQueue.clear();
        // for (int8_t i = 0; i < cellTree->maxDepth - chunkDepth; i++) {
        //     queueChunksAtDepth(i);
        // }
        // queueChunksAtDepth(0);

        // buildChunkThingies();
        // std::cout << "              Generated chunk meshes | " << 1000*(time1-time0) << "ms\n";
    }

    void setPlayer(Thingy* p) {
        player = p;
    }


    void startGeneratingWorld() {
        generatingWorld = true;
        std::jthread t1(&Gaia::updateSampleDataLoop, this);
        // std::jthread t2(&Gaia::generateSampleLoop, this);
        // std::jthread t3(&Gaia::buildChunkThingiesLoop, this);
        t1.detach();
        // t2.detach();
        // t3.detach();
    }

    void stopGeneratingWorld() {
        generatingWorld = false;
        simplifyHomogeneous();
    }


    void updateSampleDataLoop() {
        double time0;        
        double time1;
        glm::vec3 lastPlayerPos;
        while(generatingWorld) {
            std::cout << "building chunks.. " << std::endl;
            buildChunkThingies();
            if (playerTransform->position != lastPlayerPos) {
                std::cout << "updating sample data.. " << std::endl;
                lastPlayerPos = playerTransform->position;
                time0 = glfwGetTime();
                updateSampleData();
                time1 = glfwGetTime();
                std::cout << "               Generated sample data | " << 1000*(time1-time0) << "ms\n";
            }
            while(!sampleQueue.empty()) {
                std::cout << "generating sample.. " << std::endl;
                generateSample();
            }
            // while(!chunkQueue.empty()) {
            //     std::cout << "building chunk.. " << std::endl;
            //     time0 = glfwGetTime();
            //     // updateChunkFromQueue();
            //     time1 = glfwGetTime();
            //     std::cout << "                      Made new chunk | " << 1000*(time1-time0) << "ms\n";
            // }                    
        }
    }

    void generateSampleLoop() {
        double time0;
        double time1;
        while(generatingWorld) {
            if(!sampleQueue.empty()) generateSample();
        }
    }

    void buildChunkThingiesLoop() {
        double time0;
        double time1;
        while(generatingWorld) {
            if(!chunkQueue.empty()) {
                time0 = glfwGetTime();
                std::cout << "testc" << std::endl;
                updateChunkFromQueue();
                time1 = glfwGetTime();
                std::cout << "                      Made new chunk | " << 1000*(time1-time0) << "ms\n";
            }                    
        }
    }

    bool checkLOD(float distance, int8_t depth) {
        // percieved scale is a function like size/distance, 
        // so we compare  ( distance / cell size )  to a user-set LOD value
        // ideal LOD value would make distant cells the size of a pixel
        return(distance/cellTree->getCellSize(depth) < lod);
    }
    
    void updateSampleData() {
        setNeighbors();
        setBackwardsInheritedProperties();
        setSurfaces();
        for(uint8_t i = cellTree->maxDepth; i > cellTree->minDepth; i--) {
            updateSampleData(i);
        }
    }

    void updateSampleData(int8_t depth) { 
        for( auto& [position, cell] : leafMap->get(depth) ) {
            glm::vec3 cellCenter = position + glm::vec3(cellTree->getCellSize(depth-1));
            float size = cellTree->getCellSize(depth);

            
            if (depth > cellTree->minDepth && cell->surface) {
                float distance = glm::distance(cellCenter, glm::vec3(playerTransform->position.x, playerTransform->position.y, playerTransform->position.z));
                
                if (checkLOD(distance, depth)) {
                    // this cell should me upsampled
                    sampleQueue.push(CellSampleData(cell, distance, position, depth, true));
                }

            } else if (cell->parent) {
                // float dx = (cell->indexInParent >> 2) & 1;
                // float dy = (cell->indexInParent >> 1) & 1;
                // float dz = cell->indexInParent & 1;
                // glm::vec3 parentPos = position - (glm::vec3(dx, dy, dz) * size);
                // cellCenter = parentPos + glm::vec3(size);

                // float distance = glm::distance(cellCenter, glm::vec3(playerTransform->position.x, playerTransform->position.y, playerTransform->position.z));
                // if (!checkLOD(distance, depth+1)) {

                //     // parent cell should be downsampled
                //     sampleQueue.push(CellSampleData(cell->parent, distance, parentPos, depth+1, false));
                // }
            }
        }
        // std::cout << std::endl;

    }

    
    void setSurfaces() {
        setSurfaces (cellTree->root);
    }

    void setSurfaces (std::shared_ptr<OctreeNode> cell ) {


        cell->surface = (
            // cell is a surface if 
            //    1: any neighbor doesn't exist
            //    2: transparency differs between cell and any neighbor
            !cell->neighbors[0] || cell->transparent != cell->neighbors[0]->transparent  ||
            !cell->neighbors[1] || cell->transparent != cell->neighbors[1]->transparent  ||
            !cell->neighbors[2] || cell->transparent != cell->neighbors[2]->transparent  ||
            !cell->neighbors[3] || cell->transparent != cell->neighbors[3]->transparent  ||
            !cell->neighbors[4] || cell->transparent != cell->neighbors[4]->transparent  ||
            !cell->neighbors[5] || cell->transparent != cell->neighbors[5]->transparent
        );

        //  4: accounting for corner neighbors: 
        //      cell's parent is a surface, 
        //      and any two neighbors are surfaces
        //  (this is far from the best way to do this, but it will work for now)
        // if (!cell->surface && cell->parent && cell->parent->surface) {
        //     int surfaceNeighbors = 0;
        //     for (auto& neighbor : cell->neighbors) {
        //         if (neighbor && neighbor->surface) surfaceNeighbors++;

        //         if (surfaceNeighbors >= 2) {
        //             cell->surface = true;
        //             break;
        //         }
        //     }
        // }

        for (auto& child : cell->children) {
            if (child) setSurfaces(child);
        }

    }


    void generateSamples() {
        while(!sampleQueue.empty()) {
            generateSample();
        }
    }
    
    void buildChunkThingies() {
        int i = chunkQueue.size();
        int j = 0;
        while (!chunkQueue.empty()) {
            updateChunkFromQueue();
            if(j > 100) {
                std::cout << i << std::endl;
                j = 0;
            }
            i--;
            j++;
        }
    }

    void generateSample() {

        auto data = sampleQueue.top();
        std::shared_ptr<OctreeNode> cell = data.cell;

        if (!data.sampleMode) {

            // remove children / simplify cells that are to be downsampled  
            simplifyCell(cell->parent);
            if (!cell->transparent) queueChunkUpdateFromCell(CellSampleData(cell, data.distance, data.position, data.depth, true));

        } else {

            // generate children for cells to be upsampled
            float childSize = cellTree->getCellSize(data.depth-1);
            leafMap->erase(data.cell, data.depth, data.position);
            cell->leaf = false;

            for (int i = 0; i < 8; ++i) {
                auto& child = cell->children[i];
                if (!child) {
                    child = std::make_shared<OctreeNode>();
                    child->indexInParent = i;
                    child->parent = cell;
                }
                child->leaf = true;
                
                
                float dx = (i >> 2) & 1;
                float dy = (i >> 1) & 1;
                float dz = i & 1;
                
                glm::vec3 childOffset = glm::vec3(dx, dy, dz) * childSize;


                glm::vec3 childPos = data.position + childOffset;
                

                generateMatter(child, childPos + glm::vec3(cellTree->getCellSize(data.depth - 2)));
                
                leafMap->insert(child, data.depth-1, childPos);

                if (!child->transparent) queueChunkUpdateFromCell(CellSampleData(child, data.distance, childPos, data.depth-1, true));
            }


        }

        sampleQueue.pop();
    }

    void queueChunkUpdateFromCell(CellSampleData data) {
        
        std::shared_ptr<OctreeNode> chunk = data.cell;
        for(int i = 0; i < chunkDepth; i++) {
            if (chunk->parent) {
                chunk = chunk->parent; 
            } else {
                return; // FIX: later this should add to a bigger unchunked world mesh for super distant stuff that almost never changes
            }
        }


        chunkQueue.push(CellSampleData(chunk, data.distance, data.position, data.depth, true));
    }

    void updateChunkFromQueue() {
        auto data = chunkQueue.top();

        // is this chunk loaded? 
        if (chunkThingies.contains(data.cell)) {
            // yes: get chunk's object and update the mesh
        

            if (!chunkThingies.find(data.cell)->second->getComponent<GaiaChunk>().updateMesh()) {
                chunkThingies.erase(data.cell);
            }

        } else {
            // no: map chunk's calculated position, 
            //     then create an object for it
            // float chunkSize = cellTree->getCellSize(data.depth);
            // glm::vec3 chunkPos = glm::vec3(
            //     std::floor(data.position.x / chunkSize) * chunkSize,
            //     std::floor(data.position.y / chunkSize) * chunkSize,
            //     std::floor(data.position.z / chunkSize) * chunkSize
            // );
            // glm::vec3 chunkPos = glm::vec3(
            //     data.position.x,
            //     data.position.y,
            //     data.position.z
            // );
            buildChunkThingy(data.cell, data.depth);
        }
        chunkQueue.pop();
    }

    void deleteChunk(std::shared_ptr<OctreeNode> chunk) {
        chunkThingies.erase(chunk);
    }


    //     // second pass for cells with 2 surface neighbors
    //     // an imperfect but fast (for now) way to catch corners that don't get sampled properly
    //     for( auto [cell, data] : sampleQueue ) {
    //         if (!cell->leaf) break;
    //         
    //         int surfaceNeighbors = 0;
    //         for ( auto& neighbor : cell->neighbors ) {
    //             if(neighbor && !neighbor->leaf) surfaceNeighbors++;
    //             if (surfaceNeighbors > 2) {
    //                 float childSize = cellTree->getCellSize(data.depth-1);
    //                 glm::vec3 cellCenter = data.position + glm::vec3(childSize);

    //                 cell->leaf = false;

    //                 for (int i = 0; i < 8; ++i) {
    //                     auto& child = cell->children[i];
    //                     child = std::make_unique<OctreeNode>();
    //                     child->parent = cell;
    //                     child->indexInParent = i;
    //                     child->leaf = true;
    //                     
    //                     float dx = (i >> 2) & 1;
    //                     float dy = (i >> 1) & 1;
    //                     float dz = i & 1;
    //                     
    //                     glm::vec3 childOffset = glm::vec3(dx, dy, dz) * childSize;

    //                     glm::vec3 childPos = data.position + childOffset;
    //                     generateMatter(child, childPos + glm::vec3(cellTree->getCellSize(data.depth - 2)));
    //                     nextSampleData.insert({child, {childPos, data.depth - 1}});

    //                     // correct cam pos (temporary)
    //                     if(glm::distance(childPos, {playerTransform->position.x, playerTransform->position.y, playerTransform->position.z}) < cellTree->getCellSize(data.depth - 2)) {
    //                         playerTransform->position.y = child->transparent ? playerTransform->position.y - cellTree->getCellSize(data.depth - 1) : playerTransform->position.y + cellTree->getCellSize(data.depth - 1);
    //                     }
    //                 }
    //                 break;
    
    // for updating an entire depth level (lod and world loading)
    void queueChunksAtDepth(int8_t depth) {
        for( auto& [position, cell] : leafMap->get(depth) ) {
            if (!cell->surface) continue;

            std::shared_ptr<OctreeNode> chunk = cell;
            for(int i = 0; i < chunkDepth; i++) {
                if (chunk->parent) {
                    chunk = chunk->parent; 
                } else {
                    std::cout << "chunk depth too high";
                    return; // FIX: currently this never happens but later this should add to a bigger unchunked world mesh for super distant stuff that almost never changes
                }
            }

            float distance = glm::distance(playerTransform->position, position);

            chunkQueue.push(CellSampleData(chunk, distance, position, depth, true));
        }
    }

    // for updating specific chunks


    void setBackwardsInheritedProperties() {
        return setBackwardsInheritedProperties(cellTree->root);
    }
    
    void setBackwardsInheritedProperties(std::shared_ptr<OctreeNode> cell) {
        for (std::shared_ptr<OctreeNode> child : cell->children) {
            if (child) {
                if (!child->leaf) {
                    setBackwardsInheritedProperties(child);
                } else {
                    cell->transparent = cell->transparent || child->transparent;
                    cell->surface = child->surface || cell->surface;
                }
            }
        }
    }

    void setNeighbors() {
        return cellTree->setNeighbors(cellTree->root);
    }

    void setNeighbors(std::shared_ptr<OctreeNode> cell) {
        auto [depth, pos] = leafMap->get(cell);
        float size = cellTree->getCellSize(depth);
        
        // +X
        //
        for (int i = 0; i < 6; i++) {
            // use bitwise operators to calculate neighbor's position
            
            // first bit is sign, 0: positive, 1: negative
            int sign = i & 1;

            // second two bits (i shift right 1) are direction, where x=0, y=1, z=2 
            // "direction xor sign plus sign" makes it negative
            int dx = ((i >> 1 & 0) ^ -sign) + sign;             
            int dy = ((i >> 1 & 1) ^ -sign) + sign;
            int dz = ((i >> 1 & 2) ^ -sign) + sign; 
             
            glm::vec3 offset = glm::vec3(dx, dy, dz) * size;

            // find neighbor in position map 
            if (auto neighbor = leafMap->get(depth, pos + offset)) {
                cell->neighbors[i] = neighbor;
            } else {

                // backup - navigate tree to find neighbor
                // cell->neighbors[i] = findNeighbor(cell, static_cast<Direction>(i));
            }

        }
    }

    // void mapNearbyChunks(std::shared_ptr<OctreeNode>(cell), int8_t cellDepth, glm::vec3 cellPos, float cellSize) {
    //     float childSize = cellTree->getCellSize(cellDepth-1);

    //     glm::vec3 cellCenter = cellPos + glm::vec3(childSize);

    //     if(cellDepth > chunkDepth && distancefast(cellcenter, glm::vec3(campos->x, campos->y, campos->z))/cellsize < lod) {
    //         for (int i = 0; i < 8; ++i) {
    //             auto& child = cell->children[i];
    //             child = std::make_unique<OctreeNode>();
    //             child->parent = cell;
    //             child->indexInParent = i;
    //             
    //             float dx = (i >> 2) & 1;
    //             float dy = (i >> 1) & 1;
    //             float dz = i & 1;
    //             
    //             glm::vec3 childOffset = glm::vec3(dx, dy, dz) * childSize;

    //             glm::vec3 childPos = cellPos + childOffset;

    //             mapNearbyChunks(cell->children[i], cellDepth-1, childPos, childSize);
    //         }
    //         
    //         // if all children of this cell are the same, remove them and just use this one



    void simplifyHomogeneous() {
        simplifyHomogeneous(cellTree->root);
    }

    void simplifyHomogeneous(std::shared_ptr<OctreeNode> cell) {
        for (std::shared_ptr<OctreeNode> child : cell->children) {
            if (child && !child->leaf) simplifyHomogeneous(child);
        }

        for (int i = 0; i < 7; i++){
            // if any children are different from each other,
            // or if children don't exist, stop
            if ( !cell->children[i] || !cell->children[i+1] ||
                 cell->children[i] != cell->children[i+1]
            ) return;
        }
        
        simplifyCell(cell); 
    }

    void simplifyDuplicates(std::shared_ptr<OctreeNode> cell) {
        for (std::shared_ptr<OctreeNode> child : cell->children) {
            if (child) {
                simplifyDuplicates(child); // recurse into children
                simplifyDuplicates(child, cellTree->root); // compare every other cell with this child
            }
        }

    }

    void simplifyDuplicates(std::shared_ptr<OctreeNode> cell_a, std::shared_ptr<OctreeNode> cell_b) {
        for (std::shared_ptr<OctreeNode> child : cell_b->children) {
            if (child) simplifyDuplicates(cell_a, child); // recursively compare children with cell_a 
        }
        if (cell_a == cell_b) {
            // replace cell_b with cell_a 
            cell_b->parent->children[cell_b->indexInParent] = cell_a;
            for (auto& child : cell_b->children) {
                child->parent = cell_a;
            }

            leafMap->insert(cell_a, leafMap->getDepth(cell_b), leafMap->getPos(cell_b));
            leafMap->erase(cell_b);
            cell_b.reset();
        }
    }

    void simplifyCell(std::shared_ptr<OctreeNode> cell) {

        // find first existing child
        int i; 
        for(auto&& child : cell->children) {
            if (child) {
                if (!child->leaf) simplifyCell(child);


                // take its properties
                cell->transparent = child->transparent;

                // then, use its leafmap info (depth, pos) to calculate the parent's leafmap info
                i = child->indexInParent;
                int8_t depth = leafMap->getDepth(child);
                glm::vec3 childPos = leafMap->getPos(child);
                glm::vec3 pos = childPos + glm::vec3(cellTree->getCellSize(depth));
                
                // if this is the last cell in its chunk, we should unload it
                if (!child->transparent) queueChunkUpdateFromCell(CellSampleData(child, 0, childPos, depth, true));

                leafMap->erase(child, depth, childPos);
                child.reset();

                depth++;
                leafMap->insert(cell, depth, pos);
                cell->leaf = true;

                break;
            }
        }

        

        
        // now we can erase the rest of the children
        while (i < 8) {
            if (cell->children[i]) {
                if (!cell->children[i]->leaf) simplifyCell(cell->children[i]);
                leafMap->erase(cell->children[i]);
                cell->children[i].reset();
            }
            ++i;
        }

    }

    void buildChunkThingy(std::shared_ptr<OctreeNode> chunk, int8_t depth) {

        // create object for chunk
        Thingy* chunkThingy = &host->createChild("chunk");
        chunkThingy->addComponent<GaiaChunk>("GaiaChunk", chunkThingy, cellTree, leafMap, chunk, depth);

        if (chunkThingy->getComponent<GaiaChunk>().updateMesh()) {
            // map so it can be updated easily later
            std::shared_ptr<Thingy> chunkThingyShared(chunkThingy);
            chunkThingies.insert({chunk, chunkThingyShared});
        }
        
    }

    

    void generateMatter(std::shared_ptr<OctreeNode> cell, glm::vec3 blockPos) { //if(cdepth > 0 && distance3d(pos, lodpos)/csize < lod*2) { float childSize = cellSize/2.f; glm::vec3 blockPos = cellPos + glm::vec3(childSize);

       // actual terrain generation
       const float intensity = 8192.f;
       const float scale = 8192.f;

       FastNoise perlinFractal;
       perlinFractal.SetNoiseType(FastNoise::PerlinFractal); 
       perlinFractal.SetFractalOctaves(8); 
       perlinFractal.SetFrequency(1.f / scale);
       
       float noise = perlinFractal.GetValue(blockPos.x, blockPos.z);
       float mountains = (pow(noise, 1.f) ) * intensity;
       
       // flatten spawn
       mountains *= std::min(glm::distance(blockPos, {0, 0, 0}) / 64.f, 1.f);

       // spawn mountain
       const float mountainHeight = 0000;
       const float mountain = mountainHeight * (pow(0.9999f, (abs(blockPos.x) + abs(blockPos.z))));
       
       bool solid = mountains + mountain > blockPos.y;
        // bool solid = blockPos.y - (1/3) < playerTransform->position.y;

        // monolith
        // if(-1.f < blockPos.x && blockPos.x < 2.f && -1.f < blockPos.z && blockPos.z < 2.f) solid = false;
        // if( 0.f < blockPos.x && blockPos.x < 1.f &&  0.f < blockPos.z && blockPos.z < 1.f) solid = true;

        cell->transparent = !solid;

    }

};



#endif
