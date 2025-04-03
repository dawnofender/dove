#include <cmath>
#include <vector>
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

struct matter {
    //std::map<char*, float> composition; 
    bool transparent;
    bool sides[6];  // x, y, z, -x, -y, -z
    //std::vector<std::vector<std::vector<cell>>> cells;  // 3D array
};

struct cell {
    bool transparent;
    bool sides[6];  // x, y, z, -x, -y, -z
    int octave;
    std::vector<std::vector<std::vector<cell>>> cells;  // 3D array
};


struct meshData {
    std::vector<GLfloat> vData;
    std::vector<GLfloat> cData;
    int verts;
};


class Gaia{
private: 
    int size;
    int resolution;
    cell planet;
    int octaves;

public: 
    Gaia(int s, int r, int o) {
        size = s;
        resolution = r;
        octaves = o;
        generateCell(planet, 0, 0, 0, 0);
    }

    void generateCell(cell &c, int cOctave, float cx, float cy, float cz) {
        float cSize = size / resolution ^ cOctave;
        
        //generate noisy hills terrain
        //const float nScale = 64.0f;
        //const float nLacunarity = 2.0f;
        //const float nPersistance = 1.0f;
        //const int nOctaves = 5;
        //const SimplexNoise simplex(0.1f/nScale, 0.5f, nLacunarity, nPersistance);
        //const float noise = simplex.fractal(nOctaves, cx, cy, cz);
        //
        //const float solid = cy - (32 * noise) < 32;
        //c.transparent = !solid;
        c.transparent = false;
        c.octave = cOctave;
        

        if (cOctave < octaves){
            // recursively generate from smallest to largest
            c.cells = std::vector<std::vector<std::vector<cell>>> (
                resolution, std::vector<std::vector<cell>> (
                    resolution, std::vector<cell>(resolution, cell())
                )
            );
            for (int x = 0; x < resolution; x++) {
                for (int y = 0; y < resolution; y++) {
                    for (int z = 0; z < resolution; z++) {
                        generateCell(c.cells[x][y][z], cOctave+1, cSize*x+cx, cSize*y+cy, cSize*z+cz);
                    }
                }
            }
        
            // which sides should be rendered?
            for (int x = 0; x < resolution; x++) {
                for (int y = 0; y < resolution; y++) {
                    for (int z = 0; z < resolution; z++) {
                        cell &cCell = c.cells[x][y][z];
                        if(cCell.transparent){
                            std::fill(cCell.sides, cCell.sides + 6, false);
                            continue;
                        } else {
                            // check if adjascent blocks are transparent
                            // chunk are assumed to be transparent for now
                            //                 chunk edges     ||  adjascent blocks
                            cCell.sides[0] = (x == resolution - 1)  ||  c.cells[x + 1][y][z].transparent;
                            cCell.sides[1] = (y == resolution - 1)  ||  c.cells[x][y + 1][z].transparent;
                            cCell.sides[2] = (z == resolution - 1)  ||  c.cells[x][y][z + 1].transparent;                               
                            cCell.sides[3] = (x == 0)               ||  c.cells[x - 1][y][z].transparent;     
                            cCell.sides[4] = (y == 0)               ||  c.cells[x][y - 1][z].transparent;
                            cCell.sides[5] = (z == 0)               ||  c.cells[x][y][z - 1].transparent;    
                        } 
                    }
                }
            }
            
            if (cOctave == 0) {
                // planet cell values
                std::fill(c.sides, c.sides + 6, true);
            }
        }
    }
    
    meshData generateMeshData(cell &c, meshData &mesh, int &dInd, float cx, float cy, float cz) {
        float cSize = size * resolution ^ (-1 * c.octave);
        cx *= cSize;
        cy *= cSize;
        cz *= cSize;
        // float lod = size / cx;
        float lod = 1;
        
        if (c.octave / octaves < lod && c.octave < octaves) {
            for (int x = 0; x < resolution; x++) {
                for (int y = 0; y < resolution; y++) {
                    for (int z = 0; z < resolution; z++) {
                        generateMeshData(c.cells[x][y][x], mesh, dInd, x+cx, y+cy, z+cz);
                    }
                }
            }
        } else {


            static int sCount = 0;
            for (bool b : c.sides) {
                sCount += b;
            }

            mesh.verts += sCount*6; // 1 side = 2 tris = 6 verts
            mesh.vData.resize(dInd + sCount*18); // 1 side = 6 verts = 18 floats
            
            // +x
            if(c.sides[0]){

                mesh.vData[dInd]    = cx+cSize; // triangle 1
                mesh.vData[dInd+1]  = cy+cSize;
                mesh.vData[dInd+2]  = cz+cSize;
                
                mesh.vData[dInd+3]  = cx+cSize;
                mesh.vData[dInd+4]  = cy;     
                mesh.vData[dInd+5]  = cz+cSize;

                mesh.vData[dInd+6]  = cx+cSize;
                mesh.vData[dInd+7]  = cy;     
                mesh.vData[dInd+8]  = cz;     

                mesh.vData[dInd+9]  = cx+cSize; // triangle 2
                mesh.vData[dInd+10] = cy+cSize;
                mesh.vData[dInd+11] = cz+cSize;
                                      
                mesh.vData[dInd+12] = cx+cSize;
                mesh.vData[dInd+13] = cy;     
                mesh.vData[dInd+14] = cz;     
                                      
                mesh.vData[dInd+15] = cx+cSize;
                mesh.vData[dInd+16] = cy+cSize;
                mesh.vData[dInd+17] = cz;     

                for(int i = 0; i < 18; i++) {
                    mesh.cData.push_back(c.octave / octaves);
                }

                dInd += 18;
            }

            // +y
            if(c.sides[1]){
                
                mesh.vData[dInd]    = cx; // triangle 1
                mesh.vData[dInd+1]  = cy+cSize;
                mesh.vData[dInd+2]  = cz;
                                      
                mesh.vData[dInd+3]  = cx;
                mesh.vData[dInd+4]  = cy+cSize;
                mesh.vData[dInd+5]  = cz+cSize;
                                      
                mesh.vData[dInd+6]  = cx+cSize;
                mesh.vData[dInd+7]  = cy+cSize;
                mesh.vData[dInd+8]  = cz+cSize;
                                      
                mesh.vData[dInd+9]  = cx; // triangle 2
                mesh.vData[dInd+10] = cy+cSize;
                mesh.vData[dInd+11] = cz;
                                      
                mesh.vData[dInd+12] = cx+cSize;
                mesh.vData[dInd+13] = cy+cSize;
                mesh.vData[dInd+14] = cz+cSize;
                                      
                mesh.vData[dInd+15] = cx+cSize;
                mesh.vData[dInd+16] = cy+cSize;
                mesh.vData[dInd+17] = cz;

                for(int i = 0; i < 18; i++) {
                    mesh.cData.push_back(c.octave / octaves);
                }

                dInd += 18;
            }
            
            // +z
            if(c.sides[2]){
                
                mesh.vData[dInd]    = cx; // triangle 1
                mesh.vData[dInd+1]  = cy+cSize;
                mesh.vData[dInd+2]  = cz+cSize;
                                      
                mesh.vData[dInd+3]  = cx;
                mesh.vData[dInd+4]  = cy;
                mesh.vData[dInd+5]  = cz+cSize;
                                      
                mesh.vData[dInd+6]  = cx+cSize;
                mesh.vData[dInd+7]  = cy;
                mesh.vData[dInd+8]  = cz+cSize;
                                      
                mesh.vData[dInd+9]  = cx; // triangle 2
                mesh.vData[dInd+10] = cy+cSize;
                mesh.vData[dInd+11] = cz+cSize;
                                      
                mesh.vData[dInd+12] = cx+cSize;
                mesh.vData[dInd+13] = cy;
                mesh.vData[dInd+14] = cz+cSize;
                                      
                mesh.vData[dInd+15] = cx+cSize;
                mesh.vData[dInd+16] = cy+cSize;
                mesh.vData[dInd+17] = cz+cSize;

                for(int i = 0; i < 18; i++) {
                    mesh.cData.push_back(c.octave / octaves);
                }

                dInd += 18;
            }
            
            // -x
            if(c.sides[3]){
                
                mesh.vData[dInd]    = cx; // triangle 1
                mesh.vData[dInd+1]  = cy;
                mesh.vData[dInd+2]  = cz;
                                      
                mesh.vData[dInd+3]  = cx;
                mesh.vData[dInd+4]  = cy+cSize;
                mesh.vData[dInd+5]  = cz;
                                      
                mesh.vData[dInd+6]  = cx;
                mesh.vData[dInd+7]  = cy+cSize;
                mesh.vData[dInd+8]  = cz+cSize;
                                      
                mesh.vData[dInd+9]  = cx; // triangle 2
                mesh.vData[dInd+10] = cy;
                mesh.vData[dInd+11] = cz;
                                      
                mesh.vData[dInd+12] = cx;
                mesh.vData[dInd+13] = cy+cSize;
                mesh.vData[dInd+14] = cz+cSize;
                                      
                mesh.vData[dInd+15] = cx;
                mesh.vData[dInd+16] = cy;
                mesh.vData[dInd+17] = cz+cSize;

                for(int i = 0; i < 18; i++) {
                    mesh.cData.push_back(c.octave / octaves);
                }

                dInd += 18;
            }

            // -y
            if(c.sides[4]){
                
                mesh.vData[dInd]    = cx+cSize; // triangle 1
                mesh.vData[dInd+1]  = cy;
                mesh.vData[dInd+2]  = cz+cSize;
                                      
                mesh.vData[dInd+3]  = cx+cSize;
                mesh.vData[dInd+4]  = cy;
                mesh.vData[dInd+5]  = cz;
                                      
                mesh.vData[dInd+6]  = cx;
                mesh.vData[dInd+7]  = cy;
                mesh.vData[dInd+8]  = cz;
                                      
                mesh.vData[dInd+9]  = cx+cSize; // triangle 2
                mesh.vData[dInd+10] = cy;
                mesh.vData[dInd+11] = cz+cSize;
                                      
                mesh.vData[dInd+12] = cx;
                mesh.vData[dInd+13] = cy;
                mesh.vData[dInd+14] = cz;
                                      
                mesh.vData[dInd+15] = cx;
                mesh.vData[dInd+16] = cy;
                mesh.vData[dInd+17] = cz+cSize;

                for(int i = 0; i < 18; i++) {
                    mesh.cData.push_back(c.octave / octaves);
                }

                dInd += 18;
            }
            
            // -z
            if(c.sides[5]){
                
                mesh.vData[dInd]    = cx+cSize; // triangle 1
                mesh.vData[dInd+1]  = cy;
                mesh.vData[dInd+2]  = cz;
                                      
                mesh.vData[dInd+3]  = cx+cSize;
                mesh.vData[dInd+4]  = cy+cSize;
                mesh.vData[dInd+5]  = cz;
                                      
                mesh.vData[dInd+6]  = cx;
                mesh.vData[dInd+7]  = cy+cSize;
                mesh.vData[dInd+8]  = cz;
                                      
                mesh.vData[dInd+9]  = cx+cSize; // triangle 2
                mesh.vData[dInd+10] = cy;
                mesh.vData[dInd+11] = cz;
                                      
                mesh.vData[dInd+12] = cx;
                mesh.vData[dInd+13] = cy+cSize;
                mesh.vData[dInd+14] = cz;
                                      
                mesh.vData[dInd+15] = cx;
                mesh.vData[dInd+16] = cy;
                mesh.vData[dInd+17] = cz;
                
                for(int i = 0; i < 18; i++) {
                    mesh.cData.push_back(c.octave / octaves);
                }

                dInd += 18;
            }
        }
        

        //for (GLfloat d : mesh.vData){
        //    mesh.cData[i] = mesh.vData[i] / size - (1/3) + (rand() / double(RAND_MAX));
        //}
        if (c.octave == 0) {
            // mesh.cData.resize(mesh.vData.size());
            for (int i=0; i < mesh.cData.size(); i++) {
                //mesh.cData[i] = mesh.vData[i] / size - (1.0/3) + (rand() / double(RAND_MAX));
                mesh.cData[i] += mesh.vData[i] / size - (1.0/3) + (rand() / double(RAND_MAX));
                //mesh.cData[i] = rand() / double(RAND_MAX);
            }
        }

        return(mesh);
    }

    meshData generateMeshData() {
        meshData mesh;
        int ind = 0;
        return(generateMeshData(planet, mesh, ind, 0, 0, 0));
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
    
    window = glfwCreateWindow( 1024, 768, "bird dream", NULL, NULL);
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
    //glEnable(GL_CULL_FACE);     // backface culling

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
    
    Gaia world(128, 2, 4);

    meshData worldMesh = world.generateMeshData();
    

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
