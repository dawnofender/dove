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
#include <thread>
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

#include <lib/SimplexNoise.h>
#include <lib/FastNoise.h>
#include <src/dmesh/dmesh.hpp>
#include <src/worldgen/doctree.hpp>
#include <src/dthing/components/meshRendererComponent.hpp>
#include <src/dthing/components/transformComponent.hpp>
#include <src/dthing/components/playerControllerComponent.hpp>
#include <src/dthing/dthing.hpp>



struct CellSampleData {
    glm::vec3 position;
    int8_t depth;
    bool sampleMode; // true to upsample, false to downsample
    CellSampleData(glm::vec3 p, int8_t d, bool m) : position(p), depth(d), sampleMode(m) {}
};

class Gaia: public Component {
CLASS_DECLARATION(Gaia)
private: 
    Octree* cellTree;
    float planetSize;
    float lod = 16.f;
    int8_t chunkDepth = 5;
    meshData cube;
    meshData worldMesh;
    Thing* player;
    Thing* host;
    PlayerController* playerController;

    std::unordered_map<std::shared_ptr<OctreeNode>, std::vector<std::shared_ptr<OctreeNode>>> chunkMap; // chunk, leaves of chunk
    std::vector<meshData> meshes;

    std::unordered_map<std::shared_ptr<OctreeNode>, CellSampleData> sampleData;

public: 

    Gaia(std::string && initialValue, Thing* h, Thing* p)
        : Component(std::move(initialValue)), host(h), player(p) {
        
        playerController = &player->getComponent<PlayerController>();
        
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
    }
    
    void setLocalPlayer(Thing* p) {
        player = p;
        playerController = &player->getComponent<PlayerController>();
    }

    void createWorld(Octree* ot) {
        cellTree = ot;

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
        // 3) add only visible cells to the queue (next sample)
        //  - exclude cells if their percieved scale is big enough (lod) 
        //  - if player is too far away, unload cell data
        // 4) repeat 2-3 until queue is empty
        // 6) sort cells into chunks for rendering
        
        // probably combine both methds, only doing full chunk generation for chunks that are very close
        
        generateMatter(cellTree->root, cellTree->origin + glm::vec3(cellTree->getCellSize(cellTree->maxDepth-1)));
        cellTree->root->leaf = true;
        sampleData.insert({cellTree->root, CellSampleData(cellTree->origin, cellTree->maxDepth, true)});
        for (int i = cellTree->minDepth; i <= cellTree->maxDepth; i++) {
            time0 = glfwGetTime();
            updateSampleData();
            generateSample();
            time1 = glfwGetTime();
            std::cout << "          Generated world sample " << i+1 << " | " << 1000*(time1-time0) << "ms\n";
        }

        time0 = glfwGetTime();
        simplifyHomogeneous(cellTree->root);
        time1 = glfwGetTime();
        std::cout << "              Simplified homogeneous | " << 1000*(time1-time0) << "ms\n";

        time0 = glfwGetTime();
        indexLeaves();
        time1 = glfwGetTime();
        std::cout << cellTree->positionMap.size();
        std::cout << "                      Indexed Leaves | " << 1000*(time1-time0) << "ms\n";

        time0 = glfwGetTime();
        setNeighbors();
        time1 = glfwGetTime();
        std::cout << "             Neighbor data generated | " << 1000*(time1-time0) << "ms\n";

        time0 = glfwGetTime();
        indexChunks(cellTree->root);
        time1 = glfwGetTime();
        std::cout << chunkMap.size();
        std::cout << "                      Indexed chunks | " << 1000*(time1-time0) << "ms\n";

        // time0 = glfwGetTime();
        // for( auto& [chunk, leaves] : chunkMap ) {
        //     mapChunkLeaves(chunk);
        // }
        // time1 = glfwGetTime();
        // std::cout << chunkMap.size();
        // std::cout << "                 Mapped chunk leaves | " << 1000*(time1-time0) << "ms\n";

        // time0 = glfwGetTime();
        // worldMesh = buildMeshData();
        // time1 = glfwGetTime();
        // std::cout << "           World mesh Data generated | " << 1000*(time1-time0) << "ms\n";

        
        // time0 = glfwGetTime();
        // buildChunkObjects();
        // time1 = glfwGetTime();
        // std::cout << chunkMap.size();
        // std::cout << "           Chunk mesh data generated | " << 1000*(time1-time0) << "ms\n";
        
        // generateChunk(player->getPosition());
    }

    void setPlayer(Thing* p) {
        player = p;
    }

    void updateChunks() {
        while(true) {
            updateSampleData();
            generateSample();
            indexLeaves();
            setNeighbors();
            simplifyHomogeneous();
            indexChunks();
        }
    }

    bool checkLOD(float distance, int8_t depth) {
        // percieved scale is a function like size/distance, 
        // so we compare  ( distance / cell size )  to a user-set LOD value
        // ideal LOD value would make distant cells the size of a pixel
        return( distance/cellTree->getCellSize(depth) < lod && depth > 0 );
    }
    
    void updateSampleData() {
        for( auto& [cell, position] : cellTree->positionMap ) {
            int8_t depth = cellTree->depthMap.find(cell)->second;
            glm::vec3 cellCenter = position + glm::vec3(cellTree->getCellSize(depth-1));
            float size = cellTree->getCellSize(depth);
            
            if ( !cell->parent || //root
                  cell->neighbors[0] && cell->transparent != cell->neighbors[0]->transparent  ||
                  cell->neighbors[1] && cell->transparent != cell->neighbors[1]->transparent  ||
                  cell->neighbors[2] && cell->transparent != cell->neighbors[2]->transparent  ||
                  cell->neighbors[3] && cell->transparent != cell->neighbors[3]->transparent  ||
                  cell->neighbors[4] && cell->transparent != cell->neighbors[4]->transparent  ||
                  cell->neighbors[5] && cell->transparent != cell->neighbors[5]->transparent  
            ) { 
                float distance = glm::distance(cellCenter, glm::vec3(playerController->getPosition().x, playerController->getPosition().y, playerController->getPosition().z));
                if (checkLOD(distance, depth)) {
                    // this cell should me upsampled
                    sampleData.insert({cell, CellSampleData(position, depth, true)});
                }

            } else if (cell->parent) {
                float dx = (cell->indexInParent >> 2) & 1;
                float dy = (cell->indexInParent >> 1) & 1;
                float dz = cell->indexInParent & 1;
                glm::vec3 parentPos = position - glm::vec3(dx, dy, dz) * size;
                cellCenter = parentPos + glm::vec3(size);

                float distance = glm::distance(cellCenter, glm::vec3(playerController->getPosition().x, playerController->getPosition().y, playerController->getPosition().z));
                if (!checkLOD(distance, depth)) {
                    // parent cell should be downsampled
                    sampleData.insert({cell->parent, CellSampleData(parentPos, depth+1, false)});
                }
            }
        }
    }

    void generateSample() {
        std::vector<std::shared_ptr<OctreeNode>> newCells;
        for( auto [cell, data] : sampleData ) {
            if (!data.sampleMode) {

                // remove children / simplify cells that are to be downsampled  
                for ( auto& child : cell->children ) {
                    if(!child) continue;
                    cellTree->positionMap.erase(child);
                    cellTree->depthMap.erase(child);
                    child.reset();
                }
                cell->leaf = true;
                cellTree->positionMap.insert({cell, data.position});
                cellTree->depthMap.insert({cell, data.depth});

            } else {

                // generate children for cells to be upsampled
                cellTree->positionMap.erase(cell);
                cellTree->depthMap.erase(cell);
                cell->leaf = false;

                float childSize = cellTree->getCellSize(data.depth-1);
                glm::vec3 cellCenter = data.position + glm::vec3(childSize);

                for (int i = 0; i < 8; ++i) {
                    auto& child = cell->children[i];
                    if (!child) {
                        child = std::make_unique<OctreeNode>();
                        child->parent = cell;
                        child->indexInParent = i;
                        child->leaf = true;
                    }
                    
                    float dx = (i >> 2) & 1;
                    float dy = (i >> 1) & 1;
                    float dz = i & 1;
                    
                    glm::vec3 childOffset = glm::vec3(dx, dy, dz) * childSize;

                    glm::vec3 childPos = data.position + childOffset;
                    generateMatter(child, childPos + glm::vec3(cellTree->getCellSize(data.depth - 2)));

                    newCells.push_back(child);
                    cellTree->positionMap.insert({child, childPos});
                    cellTree->depthMap.insert({child, data.depth-1});

                }
            }

        }
            // clear for new data can be collected
            sampleData.clear(); 

        for( auto& cell : newCells ) {
            cellTree->setNeighbors(cell);
        }
    }

    // void makeWorldSampleData() {
    //     

    //     for( auto [cell, data] : sampleData ) {
    //         cellTree->setNeighbors(cell);
    //         
    //         float childSize = cellTree->getCellSize(data.depth-1);
    //         glm::vec3 cellCenter = data.position + glm::vec3(childSize);
    //         
    //         // if cell is root, it should be sampled
    //         // if cell's transparency is different from any of it's neighbors, and 
    //         // if it's within lod distance, it should be sampled
    //         if (
    //             !cell->parent || (( // is root
    //                     cell->neighbors[0] && cell->transparent != cell->neighbors[0]->transparent  ||
    //                     cell->neighbors[1] && cell->transparent != cell->neighbors[1]->transparent  ||
    //                     cell->neighbors[2] && cell->transparent != cell->neighbors[2]->transparent  ||
    //                     cell->neighbors[3] && cell->transparent != cell->neighbors[3]->transparent  ||
    //                     cell->neighbors[4] && cell->transparent != cell->neighbors[4]->transparent  ||
    //                     cell->neighbors[5] && cell->transparent != cell->neighbors[5]->transparent  
    //                 ) && distance(cellCenter, glm::vec3(playerController->getPosition().x, playerController->getPosition().y, playerController->getPosition().z))/cellTree->getCellSize(data.depth) < lod
    //             )
    //         ) {
    //             cell->leaf = false;

    //             // sample children
    //             for (int i = 0; i < 8; ++i) {
    //                 auto& child = cell->children[i];
    //                 child = std::make_unique<OctreeNode>();
    //                 child->parent = cell;
    //                 child->indexInParent = i;
    //                 child->leaf = true;
    //                 
    //                 float dx = (i >> 2) & 1;
    //                 float dy = (i >> 1) & 1;
    //                 float dz = i & 1;
    //                 
    //                 glm::vec3 childOffset = glm::vec3(dx, dy, dz) * childSize;

    //                 glm::vec3 childPos = data.position + childOffset;
    //                 generateMatter(child, childPos + glm::vec3(cellTree->getCellSize(data.depth - 2)));

    //                 // correct cam pos (temporary)
    //                 if(glm::distance(childPos, {player->getPosition()->x, player->getPosition()->y, player->getPosition()->z}) < cellTree->getCellSize(data.depth - 2)) {
    //                     player->getPosition()->y = child->transparent ? player->getPosition()->y - cellTree->getCellSize(data.depth - 1) : player->getPosition()->y + cellTree->getCellSize(data.depth - 1);
    //                 }
    //             }
    //         }
    //     }
    //     
    //     // second pass for cells with 2 surface neighbors
    //     // an imperfect but fast (for now) way to catch corners that don't get sampled properly
    //     for( auto [cell, data] : sampleData ) {
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
    //                     if(glm::distance(childPos, {playerController->getPosition().x, playerController->getPosition().y, playerController->getPosition().z}) < cellTree->getCellSize(data.depth - 2)) {
    //                         playerController->getPosition().y = child->transparent ? playerController->getPosition().y - cellTree->getCellSize(data.depth - 1) : playerController->getPosition().y + cellTree->getCellSize(data.depth - 1);
    //                     }
    //                 }
    //                 break;
    //             }
    //         }
    //     }
    // }

    // void generateTangibleCells() {
    //     for ( auto& [cell, depth] : cellTree->depthMap ) {
    //         if ( depth <= chunkDepth ) {
    //             glm::vec3 cellPos = cellTree->positionMap.find(cell)->second;
    //             float cellSize = cellTree->getCellSize(depth);
    //             generateWorld(cell, depth, cellPos, cellSize);
    //         }
    //     }
    //     for()
    //     cellTree->makeCell
    //     
    // }
    void indexChunks() {
        indexChunks(cellTree->root);
    };

    void indexChunks(std::shared_ptr<OctreeNode>(cell)) {
        if (!cell->leaf) {
            // recurse into children
            for ( auto& child : cell->children ) {
                if(child) indexChunks(child);
            }
        } else {
            // get chunk to index
            std::shared_ptr<OctreeNode> chunk = cell;
            for(int i = 0; i < chunkDepth; i++) {
                if (chunk->parent) {
                    chunk = chunk->parent; 
                } else {
                    return; // currently this never happens but later this should add to a bigger unchunked world mesh for super distant stuff that almost never changes
                }
            }

            // index chunk
            if(!chunkMap.contains(chunk)) {
                std::vector<std::shared_ptr<OctreeNode>> leaves;
                chunkMap.insert({chunk, leaves});
                mapChunkLeaves(chunk);
                buildChunkObject(chunk);
            }
        }
    }


    void setNeighbors() {
        return cellTree->setNeighbors(cellTree->root);
    }

    void generateWorld() {
        return generateWorld(cellTree->root, cellTree->maxDepth, cellTree->origin, planetSize);
    }
    
    void indexLeaves() {
        return cellTree->indexLeaves(cellTree->root, cellTree->maxDepth, cellTree->origin);
    }

    meshData getMesh() {
        return worldMesh;
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

    void simplifyHomogeneous(std::shared_ptr<OctreeNode>(cell)) {
        for (std::shared_ptr<OctreeNode> child : cell->children) {
            if (child && !child->leaf) simplifyHomogeneous(child);
        }

        for (int i = 0; i < 7; i++){
            // if any children are different from each other, stop
            if ( !cell->children[i] || !cell->children[i+1] ||
                 !cell->children[i]->leaf || !cell->children[i+1]->leaf ||
                  cell->children[i]->transparent != cell->children[i+1]->transparent ) return;
        }
        
        // remove children and give this their properties
        cell->transparent = cell->children[0]->transparent;
        cell->leaf = true;
        for (int i=0; i<8; i++) {
            cell->children[i].reset();
        }
    }

    // void generateChunk(glm::vec3 targetPos) {
    //     std::shared_ptr<OctreeNode> chunk;
    //     glm::vec3 position;
    //     tie(chunk, position) = cellTree->makeCell(targetPos, chunkDepth);

    //     generateChunk(chunk, chunkDepth, position);
    // }
    
    // void generateChunk(std::shared_ptr<OctreeNode> chunk, int8_t chunkDepth, glm::vec3 position) {
    //     double time0;
    //     double time1;


    //     
    //     generateRecursive(chunk, chunkDepth, position);
    //     
    //     // time0 = glfwGetTime();
    //     // simplifyHomogeneous(chunk);
    //     // time1 = glfwGetTime();
    //     // std::cout << "              Simplified homogeneous | " << 1000*(time1-time0) << "ms\n";

    //     time0 = glfwGetTime();
    //     cellTree->indexLeaves(chunk, chunkDepth, position);
    //     time1 = glfwGetTime();
    //     std::cout << "                      Indexed Leaves | " << 1000*(time1-time0) << "ms\n";

    //     time0 = glfwGetTime();
    //     mapChunkLeaves(chunk);
    //     time1 = glfwGetTime();
    //     std::cout << "                 Mapped Chunk Leaves | " << 1000*(time1-time0) << "ms\n";

    //     time0 = glfwGetTime();
    //     cellTree->setNeighbors(chunk);
    //     time1 = glfwGetTime();
    //     std::cout << "                 Set Chunk Neighbors | " << 1000*(time1-time0) << "ms\n";

    //     time0 = glfwGetTime();
    //     meshData mesh;
    //     buildChunkMesh(chunk, &mesh);

    //     time1 = glfwGetTime();
    //     std::cout << "                 Mesh Data generated | " << 1000*(time1-time0) << "ms\n";
    //     std::cout << mesh.vertices.size() << "\n";

    //     chunks.insert({chunk, mesh});
    //     
    // }
    
    void mapChunkLeaves() {
        for( auto& [chunk, leaves] : chunkMap ) {
            auto* chunkCells = &chunkMap.find(chunk)->second;
            chunkCells->clear();
            mapChunkLeaves(chunk, chunkCells, chunkDepth);
        }
    }

    void mapChunkLeaves(std::shared_ptr<OctreeNode>(chunk)) {
        auto* chunkCells = &chunkMap.find(chunk)->second;
        mapChunkLeaves(chunk, chunkCells, chunkDepth);
    }

    void mapChunkLeaves(std::shared_ptr<OctreeNode>(cell), std::vector<std::shared_ptr<OctreeNode>>* chunkCells, uint8_t localDepth) {
        if(cell->leaf && localDepth == 0) { 
            chunkCells->push_back(cell);
        } else {
            for (int i = 0; i < 8; ++i) {
                if (cell->children[i]) mapChunkLeaves(cell->children[i], chunkCells, localDepth-1);
            }
        }
    }


    void buildChunkObject(std::shared_ptr<OctreeNode>(chunk)) {
        // create mesh for the chunk
        meshData chunkMesh;
        meshes.push_back(chunkMesh);

        // create object with meshrenderer
        Thing* chunkThing = &host->createChild("chunk");
        std::shared_ptr<meshData> chunkMeshRef = std::make_shared<meshData>(chunkMesh);
        chunkThing->addComponent<MeshRenderer>("MeshRenderer", chunkMeshRef);
        buildChunkMesh(chunk, chunkMeshRef);
    }

    void buildChunkMesh(std::shared_ptr<OctreeNode>(chunk), std::shared_ptr<meshData>(mesh)) {

        auto cells = chunkMap.find(chunk)->second;
        for (auto& cell : cells) {
            buildCellMesh(cell, mesh);
        }

        if (mesh->vertices.size() < 1) {
            chunkMap.erase(chunk);
            return;
        }
        
        const float nScale = 64.f;
        const float nLacunarity = 2.f;
        const float nPersistance = 1.f;
        const int nDepth = 5.f;
        const SimplexNoise simplex(0.1f/nScale, 0.5f, nLacunarity, nPersistance);
        
        mesh->colors.resize(mesh->vertices.size());
        mesh->normals.resize(mesh->vertices.size());
        

        //calculate normals
        for (int i = 0; i < mesh->vertices.size(); i+=4) {
            glm::vec3 edge1 = mesh->vertices[i+1] - mesh->vertices[i];
            glm::vec3 edge2 = mesh->vertices[i+2] - mesh->vertices[i];
            glm::vec3 triangleNormal = normalize(cross(edge1, edge2));
            
            mesh->normals[i] = triangleNormal;
            mesh->normals[i+1] = triangleNormal;
            mesh->normals[i+2] = triangleNormal;
            mesh->normals[i+3] = triangleNormal;
        }

        // color mesh
        for (int i = 0; i < mesh->vertices.size(); i++) {
            //mesh->colors[i] = mesh->vertices[i] / size - (1.0/3) + (rand() / double(RAND_MAX));
            //mesh->colors[i] = mesh->vertices[i] / cSize - (1.0/3) + (rand() / double(RAND_MAX));
            const float noiseR = simplex.fractal(nDepth, mesh->vertices[i].x+1000.f, mesh->vertices[i].y, mesh->vertices[i].z);
            const float noiseG = simplex.fractal(nDepth, mesh->vertices[i].x+2000.f, mesh->vertices[i].y, mesh->vertices[i].z);
            const float noiseB = simplex.fractal(nDepth, mesh->vertices[i].x+3000.f, mesh->vertices[i].y, mesh->vertices[i].z);
            mesh->colors[i] = glm::vec3(noiseR + 0.5, noiseG + 0.5, noiseB + 0.5) + glm::vec3(rand() / double(RAND_MAX))/2.f;
            //mesh->colors[i] = mesh->normals[i];
        }
    }


    void buildCellMesh(std::shared_ptr<OctreeNode>(cell), std::shared_ptr<meshData>(mesh)) {

        int8_t cellDepth = cellTree->depthMap.find(cell)->second;
        glm::vec3 cellPos = cellTree->positionMap.find(cell)->second;

        // currently only rendering solid blocks
        if(!cell->transparent) {
            for(int i = 0; i < 6; i++) {
                if(cell->neighbors[i] && cell->neighbors[i]->transparent) {
                    int vertexInd = mesh->vertices.size();
                    int indexInd = mesh->indices.size();
                    int cubeInd = i*4; // which part of the cube mesh to take data from

                    mesh->vertices.resize(vertexInd + 4); // 4 vertices per side
                    mesh->indices.resize(indexInd + 6); // 6 indices per side
                    
                    GLfloat cellSize = cellTree->getCellSize(cellDepth);
                    for(int j = 0; j < 4; j++) {
                        mesh->vertices[vertexInd+j] = cellPos + cube.vertices[cubeInd+j] * cellSize;
                    }

                    mesh->indices[indexInd]   = vertexInd;
                    mesh->indices[indexInd+1] = vertexInd+1;
                    mesh->indices[indexInd+2] = vertexInd+2;
                    mesh->indices[indexInd+3] = vertexInd;
                    mesh->indices[indexInd+4] = vertexInd+2;
                    mesh->indices[indexInd+5] = vertexInd+3;
                }
            }
        }
    }
    
    // void generateRecursive(std::shared_ptr<OctreeNode>(cell), int8_t cellDepth, glm::vec3 cellPos) {
    //     float childSize = cellTree->getCellSize(cellDepth-1);
    //     glm::vec3 blockPos = cellPos + glm::vec3(childSize);

    //     if(cellDepth > cellTree->minDepth) {
    //         cell->leaf = false;

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

    //             generateRecursive(cell->children[i], cellDepth-1, cellPos + childOffset);
    //         }
    //         
    //     } else {
    //         cell->leaf = true;
    //         generateMatter(cell, blockPos);


    //     }
    // }
    

    void generateWorld(std::shared_ptr<OctreeNode>(cell), int8_t cellDepth, glm::vec3 cellPos, float cellSize) {
        float childSize = cellTree->getCellSize(cellDepth-1);

        glm::vec3 blockPos = cellPos + glm::vec3(childSize);

        if(cellDepth >= chunkDepth && distance(blockPos, glm::vec3(playerController->getPosition().x, playerController->getPosition().y, playerController->getPosition().z))/cellSize < lod) {
            cell->leaf = false;

            for (int i = 0; i < 8; ++i) {
                auto& child = cell->children[i];
                child = std::make_unique<OctreeNode>();
                child->parent = cell;
                child->indexInParent = i;
                
                float dx = (i >> 2) & 1;
                float dy = (i >> 1) & 1;
                float dz = i & 1;
                
                glm::vec3 childOffset = glm::vec3(dx, dy, dz) * childSize;

                glm::vec3 childPos = cellPos + childOffset;

                generateWorld(cell->children[i], cellDepth-1, childPos, childSize);
            }
            
            // if all children of this cell are the same, remove them and just use this one
            // simplifyHomogeneous(cell);
        } else {
            cell->leaf = true;
            generateMatter(cell, blockPos);
        }
    }
    


    void generateMatter(std::shared_ptr<OctreeNode>(cell), glm::vec3 blockPos) { //if(cdepth > 0 && distance3d(pos, lodpos)/csize < lod*2) { float childSize = cellSize/2.f; glm::vec3 blockPos = cellPos + glm::vec3(childSize);

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
        const float mountainHeight = 3000;
        const float mountain = mountainHeight * (pow(0.9999f, (abs(blockPos.x) + abs(blockPos.z))));
           
        // bool solid = (0-blockPos.y) + (1024.f * noise) + mountain > 0.f;
        bool solid = mountains + mountain > blockPos.y;

        // monolith
        // if(-1.f < blockPos.x && blockPos.x < 2.f && -1.f < blockPos.z && blockPos.z < 2.f) solid = false;
        // if( 0.f < blockPos.x && blockPos.x < 1.f &&  0.f < blockPos.z && blockPos.z < 1.f) solid = true;

        cell->transparent = !solid;
        if(cell->parent) cell->parent->transparent = cell->parent->transparent || !solid;
    }
    
    meshData buildMeshData() {
        meshData mesh;
        for(const auto& [cell, pos] : cellTree->positionMap) {
            int8_t cellDepth = cellTree->depthMap.find(cell)->second;

            // currently only rendering solid blocks
            if(!cell->transparent){ //&& cellDepth > chunkDepth) {
                for(int i = 0; i < 6; i++) {
                    if(cell->neighbors[i] && cell->neighbors[i]->transparent) {
                        int vertexInd = mesh.vertices.size();
                        int indexInd = mesh.indices.size();
                        int cubeInd = i*4; // which part of the cube mesh to take data from

                        mesh.vertices.resize(vertexInd + 4); // 4 vertices per side
                        mesh.indices.resize(indexInd + 6); // 6 indices per side
                        
                        GLfloat cellSize = cellTree->getCellSize(cellDepth);
                        for(int j = 0; j < 4; j++) {
                            mesh.vertices[vertexInd+j] = pos + cube.vertices[cubeInd+j] * cellSize;
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
        

        const float nScale = 2048.f;
        const float nLacunarity = 2.f;
        const float nPersistance = 1.f;
        const int nDepth = 5.f;
        const SimplexNoise simplex(0.1f/nScale, 0.5f, nLacunarity, nPersistance);
        
        mesh.colors.resize(mesh.vertices.size());
        mesh.normals.resize(mesh.vertices.size());
        

        //calculate normals
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
            //mesh.colors[i] = mesh.vertices[i] / size - (1.0/3) + (rand() / double(RAND_MAX));
            //mesh.colors[i] = mesh.vertices[i] / cSize - (1.0/3) + (rand() / double(RAND_MAX));
            const float noiseR = simplex.fractal(nDepth, mesh.vertices[i].x+1000.f, mesh.vertices[i].y, mesh.vertices[i].z);
            const float noiseG = simplex.fractal(nDepth, mesh.vertices[i].x+2000.f, mesh.vertices[i].y, mesh.vertices[i].z);
            const float noiseB = simplex.fractal(nDepth, mesh.vertices[i].x+3000.f, mesh.vertices[i].y, mesh.vertices[i].z);
            mesh.colors[i] = glm::vec3(noiseR + 0.5, noiseG + 0.5, noiseB + 0.5) + glm::vec3(rand() / double(RAND_MAX))/2.f;
            //mesh.colors[i] = mesh.normals[i];
        }

        return mesh;
    }

};


#endif
