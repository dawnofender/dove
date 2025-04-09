#include <cmath>
#include <vector>
#include <memory>
#include <array>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow* window;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp> // after <glm/glm.hpp>
using namespace glm;

#include <lib/shader.hpp>
#include <lib/texture.hpp>
#include <lib/controls.hpp>
#include <lib/vboindexer.hpp>

#include <lib/SimplexNoise.h>

enum Direction { POS_X = 0, NEG_X, POS_Y, NEG_Y, POS_Z, NEG_Z };

struct Matter {
    bool transparent;    
    uint8_t density;
    std::map<char*, char> properties; 
};

struct Cell {
    bool transparent;    
    bool leaf;
    Cell* parent = nullptr;        
    uint8_t indexInParent = 0;  // 0..7 as per (x<<2)|(y<<1)|z
    std::array<std::shared_ptr<Cell>, 8> children = {nullptr};
    std::array<Cell*, 6> neighbors = {nullptr}; // ±X, ±Y, ±Z
};

struct CellData{
    glm::vec3 position; //maybe try storing full translation data later? rotation for bendy living creatures etc
    int8_t depth;
    std::shared_ptr<Matter> matter;
};

struct CellTree {
    std::shared_ptr<Cell> root;
    std::unordered_map<std::shared_ptr<Cell>, CellData> leafMap;
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

Cell* findNeighbor(Cell* node, Direction dir) { //maybe keep track of distance too so we can walk all the way back down?
    if (!node->parent) return nullptr;              // hit the root, no neighbor
    uint8_t idx = node->indexInParent;
    // 1) If the neighbor is just a sibling in the same parent, return it.
    if (hasSiblingInDir(idx, dir)) {
      return node->parent->children[siblingIndex(idx,dir)].get();
    }
    // 2) Otherwise, recurse to find the parent’s neighbor in that dir
    Cell* parentNbr = findNeighbor(node->parent, dir);
    if (!parentNbr) return nullptr;
    // 3) If that neighbor isn’t subdivided, it *is* our neighbor
    if (!parentNbr->children[0])  // assume “no children” means leaf
      return parentNbr;
    // 4) Otherwise, descend into the appropriate child of that subtree
    //    which “mirrors” our index in this axis:
    return parentNbr->children[siblingIndex(idx,dir)].get();
}



struct meshData {
    std::vector<vec3> vertices;
    std::vector<vec3> colors;
    std::vector<vec3> normals;
    std::vector<unsigned int> indices;
};


class Gaia{
private: 
    Cell planet;
    CellTree cellTree;
    int8_t maxDepth; 
    vec3 origin;
    float planetSize;
    float lod = 16.f;
    vec3 camPos;
    meshData mesh;
    meshData cube;

public: 
    Gaia(vec3 p, int8_t d) {
        maxDepth = d;
        camPos = p;
        planetSize = pow(2.f, maxDepth);
        float offset = planetSize / -2.f;
        origin = vec3(
            //0,
            //0,
            //0
            offset,
            offset,
            offset
        );
        planet.leaf = false;

        cellTree.sizes.resize(maxDepth);
        for(int i = 0; i < maxDepth; i++) {
            cellTree.sizes[i] = pow(2.f, i);
        }

        cube.vertices = {
            // +x    
            vec3(1, 1, 1),
            vec3(1, 0, 1),
            vec3(1, 0, 0),
            vec3(1, 1, 0),            
            // -x    
            vec3(0, 1, 0),                
            vec3(0, 0, 0),                
            vec3(0, 0, 1),                
            vec3(0, 1, 1),                
            // +y    
            vec3(0, 1, 0),
            vec3(0, 1, 1),
            vec3(1, 1, 1),
            vec3(1, 1, 0),            
            // -y    
            vec3(0, 0, 1),                
            vec3(0, 0, 0),                
            vec3(1, 0, 0),                
            vec3(1, 0, 1),                
            // +z    
            vec3(0, 1, 1),                
            vec3(0, 0, 1),                
            vec3(1, 0, 1),                
            vec3(1, 1, 1),                          
            // -z    
            vec3(1, 1, 0),                
            vec3(1, 0, 0),                
            vec3(0, 0, 0),                
            vec3(0, 1, 0),                
        };

        cube.indices = {
            0, 1, 2, 0, 2, 3,       //+x
            4, 5, 6, 4, 6, 7,       //-x
            8, 9, 10, 8, 10, 11,    //+y
            12, 13, 14, 12, 14, 15, //-y
            16, 17, 18, 16, 18, 19, //+z
            20, 21, 22, 20, 22, 23  //-z
        };
        
        double time0 = glfwGetTime();
        generateOctree();
        double time1 = glfwGetTime();
        std::cout << "                    Octree generated | " << 1000*(time1-time0) << "ms\n";

        time0 = glfwGetTime();
        indexLeaves();
        time1 = glfwGetTime();
        std::cout << "                      Indexed leaves | " << 1000*(time1-time0) << "ms\n";

        time0 = glfwGetTime();
        generateMatter();
        time1 = glfwGetTime();
        std::cout << "                    Matter generated | " << 1000*(time1-time0) << "ms\n";

        time0 = glfwGetTime();
        setNeighbors();
        time1 = glfwGetTime();
        std::cout << "             Neighbor data generated | " << 1000*(time1-time0) << "ms\n";

        //time0 = glfwGetTime();
        //simplifyHomogeneous(&planet);
        //time1 = glfwGetTime();
        //std::cout << "   Simplified homogeneous cell trees | " << 1000*(time1-time0) << "ms\n";
        
        time0 = glfwGetTime();
        buildMeshData();
        time1 = glfwGetTime();
        std::cout << "                 Mesh Data generated | " << 1000*(time1-time0) << "ms\n";
    }

    void generateMatter() {
        return generateMatter(&planet);
    }
    void setNeighbors() {
        return setNeighbors(&planet);
    }
    void indexLeaves() {
        return indexLeaves(&planet, origin, maxDepth);
    }
    void generateOctree() {
        return generateOctree(&planet, maxDepth, origin, planetSize);
    }

    //void simplifyHomogeneous(Cell* cell) {
    //    // recurse into children first,
    //    for (auto& child : cell->children) {
    //        if(child && !child->leaf) simplifyHomogeneous(child.get());
    //    }

    //    // then simplify as we go back up

    //    for (int i = 0; i < 7; i++){
    //        // if any children are different from each other
    //        if ( !cell->children[i]->leaf || !cell->children[i+1]->leaf ||
    //              cell->children[i]->type != cell->children[i+1]->type ||
    //              cell->children[i]->transparent != cell->children[i+1]->transparent 
    //        ) return;
    //    }

    //    cell->transparent = cell->children[0]->transparent;
    //    for (int i=0; i<8; i++) {
    //       cell->children[i].reset();
    //    }
    //    cell->leaf = true;
    //}

    void indexLeaves(Cell* cell, glm::vec3 cellPos, int8_t cellDepth){
        for (auto& child: cell->children) {
            if(child) {
                // calculate position
                float dx = (child->indexInParent >> 2) & 1;
                float dy = (child->indexInParent >> 1) & 1;
                float dz = child->indexInParent & 1;
                float childSize = cellTree.sizes[cellDepth - 1];

                vec3 childOffset = vec3(
                    dx * childSize,
                    dy * childSize,
                    dz * childSize
                );
                vec3 childPos = cellPos + childOffset;
                
                if(child->leaf) {
                    // add to map for easy access 
                    cellTree.leaves.push_back(child);
                    cellTree.data.push_back({childPos, cellDepth - 1});
                } else indexLeaves(child.get(), childPos, cellDepth - 1);
            }
        }
    }
    
    meshData getMesh() {
        return mesh;
    }

    void setNeighbors(Cell* cell) {
        if (!cell) return;

        // assign neighbor pointers for this cell
        for (int d = 0; d < 6; ++d){
            cell->neighbors[d] = findNeighbor(cell, static_cast<Direction>(d));
            if (cell->neighbors[d]) std::cout << "test\n";
        }

        // recurse into children
        for (auto& child : cell->children) {
            if (child) setNeighbors(child.get());
        }
    }

    // void generateDetail(Cell* cell) {
    //     if (!cell || cell->transparent || cell->children[0]) return;

    //     for (int d = 0; d < 6; ++d) {
    //         if(cell->neighbors[d] && cell->neighbors[d]->transparent) {
    //             generateMatter(cell, 0, );
    //             for(auto& child : cell->children) {
    //                 child->visible = false; 
    //             }
    //         }       
    //     }

    //     // recurse into children
    //     for (auto& child : cell->children) {
    //         if (child) setNeighbors(child.get());
    //     }
    // }

    void generateOctree(Cell* cell, int cDepth, vec3 cellPos, float cellSize) {
        float childSize = cellSize/2.f;
        vec3 blockPos = cellPos + vec3(childSize);

        if(cDepth > 0 && distance(cellPos, camPos)/cellSize < lod) {
            cell->leaf = false;
            cell->transparent = false;

            for (int i = 0; i < 8; ++i) {
                auto& child = cell->children[i];
                child = std::make_unique<Cell>();
                child->parent = cell;
                child->indexInParent = i;
                
                float dx = (i >> 2) & 1;
                float dy = (i >> 1) & 1;
                float dz = i & 1;
                
                vec3 childOffset = vec3(
                    dx * childSize,
                    dy * childSize,
                    dz * childSize
                );

                vec3 childPos = cellPos + childOffset;

                generateOctree(cell->children[i].get(), cDepth-1, childPos, childSize);
            }
        } else cell->leaf = true;
    }

    void generateMatter(Cell* cell) { //if(cdepth > 0 && distance3d(pos, lodpos)/csize < lod*2) { float childSize = cellSize/2.f; vec3 blockPos = cellPos + vec3(childSize);

        for(const auto& leafData : cellTree.leafMap) {
            vec3 blockPos = leafData.second.position + vec3(cellTree.sizes[cellTree.data[i].depth]);
        
            // actual terrain generation
            const float nScale = 512.f;
            const float nLacunarity = 2.f;
            const float nPersistance = 1.f;
            const int nDepth = 5;
            const SimplexNoise simplex(0.1f/nScale, 0.5f, nLacunarity, nPersistance);
            const float noise = simplex.fractal(nDepth, blockPos.x, blockPos.y, blockPos.z);
            //const float noise = 0; 
            
            // spawn mountain
            const float mountainHeight = 0000;
            const float mountain = mountainHeight * (pow(0.9999f, (abs(blockPos.x) + abs(blockPos.z))));
            bool solid = (0-blockPos.y) + (1024.f * noise) + mountain > 0.f;

            // monolith
            if(-1.f < blockPos.x && blockPos.x < 2.f && -1.f < blockPos.z && blockPos.z < 2.f) solid = false;
            if( 0.f < blockPos.x && blockPos.x < 1.f &&  0.f < blockPos.z && blockPos.z < 1.f) solid = true;

            cell->transparent = !solid;
        }
        for(auto& child : cell->children) {
            if(child && child->transparent) cell->transparent = true; 
        }
    }

    void decorateMatter(Cell* cell) {
        for (auto& child : cell->children) {
            if (child) {
                decorateMatter(child.get());
                return;
            }
        }
        // generate grass and dirt
    }
    
    void buildMeshData() {
        
        // every visible cell
        for(int i = 0; i < cellTree.leaves.size(); i++) {
            // currently only rendering solid blocks
            if(!cellTree.leaves[i]->transparent) {
                for(int i = 0; i < 6; i++) {
                    if(cellTree.leaves[i]->neighbors[i] && cellTree.leaves[i]->neighbors[i]->transparent) {
                        std::cout << "test\n"; // neighbor generation probably isnt working
                        int vertexInd = mesh.vertices.size();
                        int indexInd = mesh.indices.size();
                        int cubeInd = i*4; // which part of the cube mesh to take data from
                        mesh.vertices.resize(vertexInd + 4); // 4 vertices per side
                        mesh.indices.resize(indexInd + 6); // 6 indices per side
                        for(int j = 0; j < 4; j++) {
                            mesh.vertices[vertexInd+j] = cellTree.data[i].position + cellTree.sizes[cellTree.data[i].depth] * cube.vertices[cubeInd+j];
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

        const float nScale = 64.f;
        const float nLacunarity = 2.f;
        const float nPersistance = 1.f;
        const int nDepth = 5.f;
        const SimplexNoise simplex(0.1f/nScale, 0.5f, nLacunarity, nPersistance);
        
        mesh.colors.resize(mesh.vertices.size());
        mesh.normals.resize(mesh.vertices.size());
        

        //calculate normals
        for (int i = 0; i < mesh.vertices.size(); i+=4) {
            vec3 edge1 = mesh.vertices[i+1] - mesh.vertices[i];
            vec3 edge2 = mesh.vertices[i+2] - mesh.vertices[i];
            vec3 triangleNormal = normalize(cross(edge1, edge2));
            
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
            mesh.colors[i] = vec3(noiseR + 0.5, noiseG + 0.5, noiseB + 0.5) + vec3(rand() / double(RAND_MAX))/2.f;
;
            //mesh.colors[i] = mesh.normals[i];
        }
    }

    void setCameraPosition(glm::vec3 p) {
        camPos = p;
        std::cout << camPos.x << ", " << camPos.y << ", " << camPos.z << "\n";
    }
    
};

   
int main(){
    

    // #########
    // # setup #
    // #########

    if ( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
    
    window = glfwCreateWindow( 1024, 768, "page 7.6", NULL, NULL);
    if (window == NULL){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental=true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);
	glClearColor(0.6f, 0.7f, 0.9f, 0.0f);
    glEnable(GL_DEPTH_TEST);    // Enable depth test
    glDepthFunc(GL_LESS);       // Accept fragment if it closer to the camera than the former one
    glEnable(GL_CULL_FACE);     // backface culling

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID); 

	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );

    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");


    // ############
    // # textures #
    // ############
    
    //GLuint Texture = loadDDS("uvtemplate.DDS");
    //GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
    

    // ####################
    // # model generation #
    // ####################
    
    //Gaia world(4, 2, 23);
    // Initial position : on +Z
    glm::vec3 position = glm::vec3( 0, 0, 0 ); 
    Gaia world(position, 23);
    // 23 - a bit bigger than earth
    // 90 - bigger than the observable universe (math will break and it won't generate)

    std::cout << world.getMesh().vertices.size()      << " verts\n";
    std::cout << world.getMesh().vertices.size()/3    << " tris\n";
    std::cout << world.getMesh().colors.size()        << " colors\n";
    std::cout << world.getMesh().normals.size()       << " normals\n";
    std::cout << world.getMesh().indices.size()       << " indices\n";

	
    glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, world.getMesh().vertices.size() * sizeof(vec3), &world.getMesh().vertices[0], GL_STATIC_DRAW);
    
    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, world.getMesh().colors.size() * sizeof(vec3), &world.getMesh().colors[0], GL_STATIC_DRAW);
               
    GLuint normalbuffer;
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, world.getMesh().normals.size() * sizeof(vec3), &world.getMesh().normals[0], GL_STATIC_DRAW);
    //GLuint uvbuffer;
    //glGenBuffers(1, &uvbuffer);
    //glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    //glBufferData(GL_ARRAY_BUFFER, sindexed_uvs.size() * sizeof(glm::vec3), g_uv_buffer_data, GL_STATIC_DRAW);

    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, world.getMesh().indices.size() * sizeof(unsigned int), &world.getMesh().indices[0], GL_STATIC_DRAW);
    
    int nbFrames = 0;
    
    // Initial horizontal angle : toward -Z
    float horizontalAngle = 3.14f;
    // Initial vertical angle : none
    float verticalAngle = 0.0f;
    // Initial Field of View
    float initialFoV = 90.0f;
    
    float speed = 32.0f; // 3 units / second
    float mouseSpeed = 0.005f;
    float near = 0.01f;
    float far = 12000.0f;



    double lastFrameTime = glfwGetTime();
    double lastGenTime = lastFrameTime;
    do{
        // measure fps
		double currentTime = glfwGetTime();
		nbFrames++;
		if ( currentTime - lastFrameTime >= 1.0 ){ // If last prinf() was more than 1sec ago
			// printf and reset
			printf("%f ms/frame    ", 1000.0/double(nbFrames));
            std:: cout << position.x << ", " << position.y << ", " << position.z << "\n";
			nbFrames = 0;
			lastFrameTime += 1.0;
		}

        if ( currentTime - lastGenTime >= 10000005.0 ) {
            world.setCameraPosition(position);
            world.generateMatter();
            world.setNeighbors();
            world.buildMeshData();
            lastGenTime += 15.0;
            
            
	        glBufferData(GL_ARRAY_BUFFER, world.getMesh().vertices.size() * sizeof(vec3) , &world.getMesh().vertices[0], GL_STATIC_DRAW);
	        glBufferData(GL_ARRAY_BUFFER, world.getMesh().colors.size() * sizeof(vec3) , &world.getMesh().colors[0], GL_STATIC_DRAW);
            glBufferData(GL_ARRAY_BUFFER, world.getMesh().normals.size() * sizeof(vec3) , &world.getMesh().normals[0], GL_STATIC_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, world.getMesh().indices.size() * sizeof(unsigned int), &world.getMesh().indices[0], GL_STATIC_DRAW);
        }
        
        

        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(programID);

        // ##########
        // # camera #
        // ##########
       
        computeMatricesFromInputs(
            position,
            horizontalAngle,
            verticalAngle,
            initialFoV,
            speed,
            mouseSpeed,
            near,
            far 
        );

        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;


        // Send our transformation to the currently bound shader, 
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

        vec3 lightPos = vec3(0,10000,0);
        glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
        
        // vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void*)0
        );

        // colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

         // 3rd attribute buffer : normals
         glEnableVertexAttribArray(2);
         glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
         glVertexAttribPointer(
             2,                                // attribute
             3,                                // size
             GL_FLOAT,                         // type
             GL_FALSE,                         // normalized?
             0,                                // stride
             (void*)0                          // array buffer offset
         );
       
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        
        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        
        // actually draw things :)
        //glDrawArrays(GL_TRIANGLES, 0, world.getMesh().vertices.size());
        glDrawElements(
            GL_TRIANGLES,      // mode
            world.getMesh().indices.size(),    // count
            GL_UNSIGNED_INT,   // type
            (void*)0           // element array buffer offset
        );

        glDisableVertexAttribArray(0);
	    glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );
    
    glDeleteBuffers(1, &vertexbuffer);
    //glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &colorbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);
    glDeleteProgram(programID);
    //glDeleteTextures(1, &Texture);
    glDeleteVertexArrays(1, &VertexArrayID);

    glfwTerminate();
	
    return 0;
}
