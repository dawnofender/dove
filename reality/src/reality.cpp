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

struct cell {
    int type;
    bool visible;
    bool transparent;
    bool sides[6];  // x, y, z, -x, -y, -z
};

struct meshData {
    std::vector<GLfloat> vData;
    std::vector<GLfloat> cData;
    int tris;
};

float mix(const float a, const float b, const float c) {
    return(a * (1 - c) + (b * c));
}


class Cell {
private: 
    int type;
    bool visible;
    bool transparent;
    bool sides[6];

    int size;
    int res;
    std::vector<std::vector<std::vector<Cell>>> cells;  // 3D array

public: 

    Cell(int t, bool transparent, )
    Fabric(int s, int r) {
        size = s;
        res = r;
    }
};


class Gaia: private Cell {
public: 
    Gaia(int s, int o) {
        size = s;
        octaves = o;
        generateCells();
    }

    void generateCells() {

        cells = std::vector<std::vector<std::vector<cell>>> (
            size, std::vector<std::vector<cell>> (
                size, std::vector<cell>(size, cell())
            )
        );

        const float scale = 64.0f;
        const float lacunarity = 2.0f;
        const float persistance = 1.0f;
        const int octaves = 5;
        const SimplexNoise simplex(0.1f/scale, 0.5f, lacunarity, persistance);

        // generate wavy terrain
        // cell types first

        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {
                for (int z = 0; z < size; z++) {

                    //if(j < sin(i/3)*4 + size / 2) {
                    //    cells[i][j][k].type = 1;
                    //    cells[i][j][k].transparent = false;
                    //} else {
                    //    cells[i][j][k].type = 0;
                    //    cells[i][j][k].transparent = true;
                    //}

                    //const float noise = simplex.noise(i, j, k); 
                    const float noise = simplex.fractal(octaves, x, y, z);
                    const float earth = y - (32 * noise) < 32;
                    if (earth) {
                        cells[x][y][z].type = 1;
                        cells[x][y][z].transparent = false;
                    } else {
                        cells[x][y][z].type = 0;
                        cells[x][y][z].transparent = true;
                    }

                    //if (j > size / 2) {
                    //    cells[i][j][k].type = 1;
                    //    cells[i][j][k].transparent = false;
                    //}

                    //cells[i][j][k].type = 1;
                    //cells[i][j][k].transparent = false;

                }
            }
        }

        // which sides should be rendered? 
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                for (int k = 0; k < size; k++) {
                    if(cells[i][j][k].transparent){
                        std::fill(cells[i][j][k].sides, cells[i][j][k].sides + 6, false);
                        continue;
                    } else {
                        // check if adjascent blocks are transparent
                        // chunk are assumed to be transparent for now
                        //                        chunk edges     ||  adjascent blocks
                        cells[i][j][k].sides[0] = (i == size - 1) ||  cells[i + 1][j][k].transparent;
                        cells[i][j][k].sides[1] = (j == size - 1) ||  cells[i][j + 1][k].transparent;
                        cells[i][j][k].sides[2] = (k == size - 1) ||  cells[i][j][k + 1].transparent;                               
                        cells[i][j][k].sides[3] = (i == 0)        ||  cells[i - 1][j][k].transparent;     
                        cells[i][j][k].sides[4] = (j == 0)        ||  cells[i][j - 1][k].transparent;
                        cells[i][j][k].sides[5] = (k == 0)        ||  cells[i][j][k - 1].transparent;    
                    } 
                }
            }
        }

    }
    
    meshData generateMeshData() {

        // first, count how many vertices we need
        int sCount = 0;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                for (int k = 0; k < size; k++) {
                    if (cells[i][j][k].transparent){
                        continue;
                    } else {
                        for (bool b : cells[i][j][k].sides) {
                            sCount += b;
                        }
                    }
                }
            }
        }
        
        static int tCount = sCount * 2;
        static int dCount = sCount * 18; // 2 tris * 3 vertices * 3 floats
        std::vector<GLfloat> vData(dCount);         
        std::vector<GLfloat> cData(dCount);         

        int dInd = 0;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                for (int k = 0; k < size; k++) {
                    if (cells[i][j][k].transparent){
                        continue;
                    } else {
                        // +x
                        if(cells[i][j][k].sides[0]){
                            vData[dInd]    = i+1.0f; // triangle 1
                            vData[dInd+1]  = j+1.0f;
                            vData[dInd+2]  = k+1.0f;
                            
                            vData[dInd+3]  = i+1.0f;
                            vData[dInd+4]  = j;
                            vData[dInd+5]  = k+1.0f;

                            vData[dInd+6]  = i+1.0f;
                            vData[dInd+7]  = j;
                            vData[dInd+8]  = k;
                            
                            vData[dInd+9]  = i+1.0f; // triangle 2
                            vData[dInd+10] = j+1.0f;
                            vData[dInd+11] = k+1.0f;
                            
                            vData[dInd+12] = i+1.0f;
                            vData[dInd+13] = j;
                            vData[dInd+14] = k;

                            vData[dInd+15] = i+1.0f;
                            vData[dInd+16] = j+1.0f;
                            vData[dInd+17] = k;

                            dInd += 18;
                        }

                        // +y
                        if(cells[i][j][k].sides[1]){
                            
                            vData[dInd]    = i; // triangle 1
                            vData[dInd+1]  = j+1.0f;
                            vData[dInd+2]  = k;
                                              
                            vData[dInd+3]  = i;
                            vData[dInd+4]  = j+1.0f;
                            vData[dInd+5]  = k+1.0f;
                                              
                            vData[dInd+6]  = i+1.0f;
                            vData[dInd+7]  = j+1.0f;
                            vData[dInd+8]  = k+1.0f;
                                              
                            vData[dInd+9]  = i; // triangle 2
                            vData[dInd+10] = j+1.0f;
                            vData[dInd+11] = k;
                                              
                            vData[dInd+12] = i+1.0f;
                            vData[dInd+13] = j+1.0f;
                            vData[dInd+14] = k+1.0f;
                                              
                            vData[dInd+15] = i+1.0f;
                            vData[dInd+16] = j+1.0f;
                            vData[dInd+17] = k;

                            dInd += 18;
                        }
                        
                        // +z
                        if(cells[i][j][k].sides[2]){
                            
                            vData[dInd]    = i; // triangle 1
                            vData[dInd+1]  = j+1.0f;
                            vData[dInd+2]  = k+1.0f;
                                              
                            vData[dInd+3]  = i;
                            vData[dInd+4]  = j;
                            vData[dInd+5]  = k+1.0f;
                                              
                            vData[dInd+6]  = i+1.0f;
                            vData[dInd+7]  = j;
                            vData[dInd+8]  = k+1.0f;
                                              
                            vData[dInd+9]  = i; // triangle 2
                            vData[dInd+10] = j+1.0f;
                            vData[dInd+11] = k+1.0f;
                                              
                            vData[dInd+12] = i+1.0f;
                            vData[dInd+13] = j;
                            vData[dInd+14] = k+1.0f;
                                              
                            vData[dInd+15] = i+1.0f;
                            vData[dInd+16] = j+1.0f;
                            vData[dInd+17] = k+1.0f;

                            dInd += 18;
                        }
                        
                        // -x
                        if(cells[i][j][k].sides[3]){
                            
                            vData[dInd]    = i; // triangle 1
                            vData[dInd+1]  = j;
                            vData[dInd+2]  = k;
                                              
                            vData[dInd+3]  = i;
                            vData[dInd+4]  = j+1.0f;
                            vData[dInd+5]  = k;
                                              
                            vData[dInd+6]  = i;
                            vData[dInd+7]  = j+1.0f;
                            vData[dInd+8]  = k+1.0f;
                                              
                            vData[dInd+9]  = i; // triangle 2
                            vData[dInd+10] = j;
                            vData[dInd+11] = k;
                                              
                            vData[dInd+12] = i;
                            vData[dInd+13] = j+1.0f;
                            vData[dInd+14] = k+1.0f;
                                              
                            vData[dInd+15] = i;
                            vData[dInd+16] = j;
                            vData[dInd+17] = k+1.0f;

                            dInd += 18;
                        }

                        // -y
                        if(cells[i][j][k].sides[4]){
                            
                            vData[dInd]    = i+1.0f; // triangle 1
                            vData[dInd+1]  = j;
                            vData[dInd+2]  = k+1.0f;
                                              
                            vData[dInd+3]  = i+1.0f;
                            vData[dInd+4]  = j;
                            vData[dInd+5]  = k;
                                              
                            vData[dInd+6]  = i;
                            vData[dInd+7]  = j;
                            vData[dInd+8]  = k;
                                              
                            vData[dInd+9]  = i+1.0f; // triangle 2
                            vData[dInd+10] = j;
                            vData[dInd+11] = k+1.0f;
                                              
                            vData[dInd+12] = i;
                            vData[dInd+13] = j;
                            vData[dInd+14] = k;
                                              
                            vData[dInd+15] = i;
                            vData[dInd+16] = j;
                            vData[dInd+17] = k+1.0f;

                            dInd += 18;
                        }
                        
                        // -z
                        if(cells[i][j][k].sides[5]){
                            
                            vData[dInd]    = i+1.0f; // triangle 1
                            vData[dInd+1]  = j;
                            vData[dInd+2]  = k;
                                              
                            vData[dInd+3]  = i+1.0f;
                            vData[dInd+4]  = j+1.0f;
                            vData[dInd+5]  = k;
                                              
                            vData[dInd+6]  = i;
                            vData[dInd+7]  = j+1.0f;
                            vData[dInd+8]  = k;
                                              
                            vData[dInd+9]  = i+1.0f; // triangle 2
                            vData[dInd+10] = j;
                            vData[dInd+11] = k;
                                              
                            vData[dInd+12] = i;
                            vData[dInd+13] = j+1.0f;
                            vData[dInd+14] = k;
                                              
                            vData[dInd+15] = i;
                            vData[dInd+16] = j;
                            vData[dInd+17] = k;
                            
                            dInd += 18;
                        }
                    }
                }
            }
        }

        for (int i = 0; i < dCount; i++){
            cData[i] = vData[i] / size - (1/3) + (rand() / double(RAND_MAX));
        }
        
        meshData mesh = {vData, cData, tCount};
        return(mesh);
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
    
    Fabric world(256);
    
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
        glDrawArrays(GL_TRIANGLES, 0, worldMesh.tris * 3);
        
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
