#include <cmath>
#include <vector>
#include <memory>
#include <array>
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

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/vboindexer.hpp>

#include <common/SimplexNoise.h>

// better cell class - implement later 
//class Cell {
//protected: 
//    std::vector<std::shared_ptr<Organelle>> organelles;
//public: 
//    Cell() = default;
//    Cell(std::array<Organelle> o) {
//        organelles = o;
//    }
//    
//    void AddComponent(std::shared_ptr<Component> component) {
//
//    }
//
//    Organelle getOrganelle(int i) {
//        return organelles[i];
//    }
//};
struct Vec3 {
    float x, y, z;
};

inline float distance3D(const Vec3& a, const Vec3& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}


inline double distance3D(int x1, int y1, int z1, int x2, int y2, int z2) {
    int dx = x1 - x2;
    int dy = y1 - y2;
    int dz = z1 - z2;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}


struct matter {
    //std::map<char*, float> composition; 
    bool transparent;
    bool sides[6];  // x, y, z, -x, -y, -z
    //std::vector<std::vector<std::vector<cell>>> cells;  // 3D array
};

enum Direction { POS_X = 0, NEG_X, POS_Y, NEG_Y, POS_Z, NEG_Z };


struct Cell {
    bool visible;
    bool transparent;    

    Cell* parent = nullptr;        
    int indexInParent = 0;  // 0..7 as per (x<<2)|(y<<1)|z

    std::array<std::unique_ptr<Cell>, 8> children = {nullptr};
    std::array<Cell*, 6> neighbors = {nullptr}; // ±X, ±Y, ±Z
};


// given a node index (0–7) and a direction, can we move within the same parent?
bool hasSiblingInDir(int idx, Direction d) {
    int x = (idx>>2)&1, y = (idx>>1)&1, z = idx&1;
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
int siblingIndex(int idx, Direction d) {
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

Cell* findNeighbor(Cell* node, Direction dir) {
    if (!node->parent) return nullptr;              // hit the root, no neighbor
    int idx = node->indexInParent;
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

void cellOffset(int index, int& x, int& y, int& z) {
    x = (index >> 2) & 1;
    y = (index >> 1) & 1;
    z = index & 1;
}

struct meshData {
    std::vector<GLfloat> vData;
    std::vector<GLfloat> cData;
    int verts;
};



class Gaia{
private: 
    Cell planet;
    int depth;

    float lod = 5.0;
    Vec3 lodPos = {16, 47, 16};
    meshData mesh;

public: 
    Gaia(int d) {
        depth = d;
        float planetSize = pow(2, depth);
        Vec3 worldPos = {
            -1000,
            0,
            -1000

        };
        
        double time0 = glfwGetTime();
        generateMatter(&planet, depth, planetSize, worldPos);
        double time1 = glfwGetTime();
        std::cout << "Matter generated in " << 1000*(time1-time0) << "ms\n";

        time0 = glfwGetTime();
        setNeighbors(&planet);
        time1 = glfwGetTime();
        std::cout << "Neighbor data generated in " << 1000*(time1-time0) << "ms\n";
        
        time0 = glfwGetTime();
        buildMeshData(&planet, depth, planetSize, worldPos);
        time1 = glfwGetTime();
        
        std::cout << "Mesh Data generated in " << 1000*(time1-time0) << "ms\n";
    }
    
    meshData getMesh() {
        return mesh;
    }

    void setNeighbors(Cell* cell) {
        if (!cell) return;

        // assign neighbor pointers for this cell
        for (int d = 0; d < 6; ++d) {
            cell->neighbors[d] = findNeighbor(cell, static_cast<Direction>(d));
        }

        // recurse into children
        for (auto& child : cell->children) {
            if (child) setNeighbors(child.get());
        }
    }

    void generateMatter(Cell* cell, int cDepth, float cSize, Vec3 pos) {
        //if(cdepth > 0 && distance3d(pos, lodpos)/csize < lod*2) {
        float childSize = cSize/2;
        
        Vec3 blockPos = {
            pos.x + childSize,
            pos.y + childSize,
            pos.z + childSize

        };

        if(cDepth > 0 && distance3D(blockPos, lodPos)/cSize < lod*2) {
            cell->visible = false;
            for (int i = 0; i < 8; ++i) {

                float dx = (i >> 2) & 1;
                float dy = (i >> 1) & 1;
                float dz = i & 1;
                Vec3 childPos = {
                    pos.x + dx * childSize,
                    pos.y + dy * childSize,
                    pos.z + dz * childSize
                };
                float childSize = cSize/2;
                cell->children[i] = std::make_unique<Cell>();
                cell->children[i]->parent = cell;
                cell->children[i]->indexInParent = i;
                generateMatter(cell->children[i].get(), cDepth-1, childSize, childPos);
            }
        } else cell->visible = true;

        // actual terrain generation
        const float nScale = 64.0f;
        const float nLacunarity = 2.0f;
        const float nPersistance = 1.0f;
        const int ndepth = 5;
        const SimplexNoise simplex(0.1f/nScale, 0.5f, nLacunarity, nPersistance);
        const float noise = simplex.fractal(ndepth, blockPos.x, blockPos.y, blockPos.z);
        const float solid = blockPos.y - (128 * noise) < 32;
        cell->transparent = !solid;
    }
    
    void buildMeshData(Cell* cell, int cDepth, float cSize, Vec3 pos) {
        if(!cell) return;
        // float lod = size / pos.x);
        //
        //
        if(!cell->visible && cDepth > 0) {
            float childSize = cSize/2;
            for (int i = 0; i < 8; ++i){
                float dx = (i >> 2) & 1;
                float dy = (i >> 1) & 1;
                float dz = i & 1;

                Vec3 childPos = {
                    pos.x + dx * childSize,
                    pos.y + dy * childSize,
                    pos.z + dz * childSize
                };
                buildMeshData(cell->children[i].get(), cDepth-1, childSize, childPos);
            }
        }

        if(cell->visible && !cell->transparent) {
            // +x
            if(cell->neighbors[0] && cell->neighbors[0]->transparent) {
                mesh.vData.push_back(pos.x+cSize);
                mesh.vData.push_back(pos.y+cSize);
                mesh.vData.push_back(pos.z+cSize);

                mesh.vData.push_back(pos.x+cSize);
                mesh.vData.push_back(pos.y);
                mesh.vData.push_back(pos.z+cSize);

                mesh.vData.push_back(pos.x+cSize);
                mesh.vData.push_back(pos.y);
                mesh.vData.push_back(pos.z);

                mesh.vData.push_back(pos.x+cSize);
                mesh.vData.push_back(pos.y+cSize);
                mesh.vData.push_back(pos.z+cSize);
                
                mesh.vData.push_back(pos.x+cSize);
                mesh.vData.push_back(pos.y);
                mesh.vData.push_back(pos.z);

                mesh.vData.push_back(pos.x+cSize);
                mesh.vData.push_back(pos.y+cSize);
                mesh.vData.push_back(pos.z);
            }

            // -x         
            if(cell->neighbors[1] && cell->neighbors[1]->transparent) {
                          
                mesh.vData.push_back(pos.x); // triangle 1
                mesh.vData.push_back(pos.y);
                mesh.vData.push_back(pos.z);
                   
                mesh.vData.push_back(pos.x);
                mesh.vData.push_back(pos.y+cSize);
                mesh.vData.push_back(pos.z+cSize);
                   
                mesh.vData.push_back(pos.x);
                mesh.vData.push_back(pos.y+cSize);
                mesh.vData.push_back(pos.z);
                   
                mesh.vData.push_back(pos.x); // triangle 2
                mesh.vData.push_back(pos.y);
                mesh.vData.push_back(pos.z);
                   
                mesh.vData.push_back(pos.x);
                mesh.vData.push_back(pos.y);
                mesh.vData.push_back(pos.z+cSize);
                   
                mesh.vData.push_back(pos.x);
                mesh.vData.push_back(pos.y+cSize);
                mesh.vData.push_back(pos.z+cSize);
            }             
            // +y
            if(cell->neighbors[2] && cell->neighbors[2]->transparent) {
                
                mesh.vData.push_back(pos.x);
                mesh.vData.push_back(pos.y+cSize);
                mesh.vData.push_back(pos.z);

                mesh.vData.push_back(pos.x);
                mesh.vData.push_back(pos.y+cSize);
                mesh.vData.push_back(pos.z+cSize);

                mesh.vData.push_back(pos.x+cSize);
                mesh.vData.push_back(pos.y+cSize);
                mesh.vData.push_back(pos.z+cSize);

                mesh.vData.push_back(pos.x);
                mesh.vData.push_back(pos.y+cSize);
                mesh.vData.push_back(pos.z);

                mesh.vData.push_back(pos.x+cSize);
                mesh.vData.push_back(pos.y+cSize);
                mesh.vData.push_back(pos.z+cSize);

                mesh.vData.push_back(pos.x+cSize);
                mesh.vData.push_back(pos.y+cSize);
                mesh.vData.push_back(pos.z);
            }
            
            // -y         
            if(cell->neighbors[3] && cell->neighbors[3]->transparent) {
                          
                mesh.vData.push_back(pos.x+cSize); // triangle 1
                mesh.vData.push_back(pos.y);
                mesh.vData.push_back(pos.z+cSize);
                   
                mesh.vData.push_back(pos.x);
                mesh.vData.push_back(pos.y);
                mesh.vData.push_back(pos.z);
                   
                mesh.vData.push_back(pos.x+cSize);
                mesh.vData.push_back(pos.y);
                mesh.vData.push_back(pos.z);
                   
                mesh.vData.push_back(pos.x+cSize); // triangle 2
                mesh.vData.push_back(pos.y);
                mesh.vData.push_back(pos.z+cSize);
                   
                mesh.vData.push_back(pos.x);
                mesh.vData.push_back(pos.y);
                mesh.vData.push_back(pos.z+cSize);
                   
                mesh.vData.push_back(pos.x);
                mesh.vData.push_back(pos.y);
                mesh.vData.push_back(pos.z);
            }             
                          
            // +z
            if(cell->neighbors[4] && cell->neighbors[4]->transparent) {
                
                mesh.vData.push_back(pos.x); // triangle 1
                mesh.vData.push_back(pos.y+cSize);
                mesh.vData.push_back(pos.z+cSize);
                   
                mesh.vData.push_back(pos.x);
                mesh.vData.push_back(pos.y);
                mesh.vData.push_back(pos.z+cSize);
                   
                mesh.vData.push_back(pos.x+cSize);
                mesh.vData.push_back(pos.y);
                mesh.vData.push_back(pos.z+cSize);
                   
                mesh.vData.push_back(pos.x); // triangle 2
                mesh.vData.push_back(pos.y+cSize);
                mesh.vData.push_back(pos.z+cSize);
                   
                mesh.vData.push_back(pos.x+cSize);
                mesh.vData.push_back(pos.y);
                mesh.vData.push_back(pos.z+cSize);
                   
                mesh.vData.push_back(pos.x+cSize);
                mesh.vData.push_back(pos.y+cSize);
                mesh.vData.push_back(pos.z+cSize);
            }

                          
            // -z         
            if(cell->neighbors[5] && cell->neighbors[5]->transparent){
                          
                mesh.vData.push_back(pos.x+cSize); // triangle 1
                mesh.vData.push_back(pos.y);
                mesh.vData.push_back(pos.z);
                   
                mesh.vData.push_back(pos.x);
                mesh.vData.push_back(pos.y+cSize);
                mesh.vData.push_back(pos.z);
                   
                mesh.vData.push_back(pos.x+cSize);
                mesh.vData.push_back(pos.y+cSize);
                mesh.vData.push_back(pos.z);
                   
                mesh.vData.push_back(pos.x+cSize); // triangle 2
                mesh.vData.push_back(pos.y);
                mesh.vData.push_back(pos.z);
                   
                mesh.vData.push_back(pos.x);
                mesh.vData.push_back(pos.y);
                mesh.vData.push_back(pos.z);
                   
                mesh.vData.push_back(pos.x);
                mesh.vData.push_back(pos.y+cSize);
                mesh.vData.push_back(pos.z);
            }
        }


        if (cDepth == depth) {

            const float nScale = 64.0f;
            const float nLacunarity = 2.0f;
            const float nPersistance = 1.0f;
            const int ndepth = 5;
            const SimplexNoise simplex(0.1f/nScale, 0.5f, nLacunarity, nPersistance);
            
            mesh.cData.resize(mesh.vData.size());
            for (int i=0; i < mesh.vData.size(); i+=3) {
                //mesh.cData[i] = mesh.vData[i] / size - (1.0/3) + (rand() / double(RAND_MAX));
                mesh.cData[i] = mesh.vData[i] / cSize - (1.0/3) + (rand() / double(RAND_MAX));
                //mesh.cData.push_back(rand() / double(RAND_MAX));
                const float noiseR = simplex.fractal(ndepth, mesh.vData[i]+1000, mesh.vData[i+1], mesh.vData[i+2]);
                const float noiseG = simplex.fractal(ndepth, mesh.vData[i]+2000, mesh.vData[i+1], mesh.vData[i+2]);
                const float noiseB = simplex.fractal(ndepth, mesh.vData[i]+3000, mesh.vData[i+1], mesh.vData[i+2]);
                mesh.cData[i] = noiseR + 0.5;
                mesh.cData[i+1] = noiseG + 0.5;
                mesh.cData[i+2] = noiseB + 0.5;
                //mesh.cData[i] = rand() / double(RAND_MAX);
            }
        }
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


    // ############
    // # textures #
    // ############
    
    //GLuint Texture = loadDDS("uvtemplate.DDS");
    //GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
    

    // ####################
    // # model generation #
    // ####################
    
    //Gaia world(4, 2, 23);
    Gaia world(21);
    meshData worldMesh = world.getMesh();

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * worldMesh.vData.size(), &worldMesh.vData[0], GL_STATIC_DRAW);
    
    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * worldMesh.cData.size(), &worldMesh.cData[0], GL_STATIC_DRAW);
    
    //GLuint uvbuffer;
    //glGenBuffers(1, &uvbuffer);
    //glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    //glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec3), g_uv_buffer_data, GL_STATIC_DRAW);

    //GLuint elementbuffer;
    //glGenBuffers(1, &elementbuffer);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    
    do{
        // measure fps
		double currentTime = glfwGetTime();
		nbFrames++;
		if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
			// printf and reset
			printf("%f ms/frame\n", 1000.0/double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(programID);

        // ##########
        // # camera #
        // ##########
       
        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

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

        // 2nd attribute buffer: uvs
        //glEnableVertexAttribArray(1);
        //glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        //glVertexAttribPointer(
        //    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        //    2,                                // size
        //    GL_FLOAT,                         // type
        //    GL_FALSE,                         // normalized?
        //    0,                                // stride
        //    (void*)0                          // array buffer offset
        //);
 
        // 3nd attribute buffer : colors
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
       
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        
        // actually draw things :)
        glDrawArrays(GL_TRIANGLES, 0, worldMesh.verts);
        
        //glDrawElements(
        //    GL_TRIANGLES,      // mode
        //    indices.size(),    // count
        //    GL_UNSIGNED_INT,   // type
        //    (void*)0           // element array buffer offset
        //);

        glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );
    
    glDeleteBuffers(1, &vertexbuffer);
    //glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &colorbuffer);
	//glDeleteBuffers(1, &normalbuffer);
	//glDeleteBuffers(1, &elementbuffer);
    glDeleteProgram(programID);
    //glDeleteTextures(1, &Texture);
    glDeleteVertexArrays(1, &VertexArrayID);

    glfwTerminate();
	
    return 0;
}
