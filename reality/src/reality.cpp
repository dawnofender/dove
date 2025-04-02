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
        generateCell(planet, octaves, 0, 0, 0);
    }

    void generateCell(cell &c, int cOctave, int x, int y, int z) {

        c.cells = std::vector<std::vector<std::vector<cell>>> (
            size, std::vector<std::vector<cell>> (
                size, std::vector<cell>(size, cell())
            )
        );

        std::cout << "Generating cell at (";
        std::cout <<  x;
        std::cout <<  ", ";
        std::cout <<  y;
        std::cout <<  ", ";
        std::cout <<  z;
        std::cout <<  "), octave ";
        std::cout <<  cOctave;
        std::cout <<  "\n";


        //generate noisy hills terrain
        const float nScale = 64.0f;
        const float nLacunarity = 2.0f;
        const float nPersistance = 1.0f;
        const int nOctaves = 5;
        const SimplexNoise simplex(0.1f/nScale, 0.5f, nLacunarity, nPersistance);
        const float noise = simplex.fractal(nOctaves, x, y, z);
        
        const float solid = y - (32 * noise) < 32;
        c.transparent = !solid;

        c.octave = cOctave;


        if (cOctave > 0){
            for (int x = 0; x < size; x++) {
                for (int y = 0; y < size; y++) {
                    for (int z = 0; z < size; z++) {
                        if (cOctave > 0){
                            generateCell(c.cells[x][y][z], cOctave-1, x, y, z);
                        }
                    }
                }
            }
        }

        std::cout << "Done generating cell at (";
        std::cout <<  x;
        std::cout <<  ", ";
        std::cout <<  y;
        std::cout <<  ", ";
        std::cout <<  z;
        std::cout <<  "), octave ";
        std::cout <<  cOctave;
        std::cout <<  "\n";

        // which sides should be rendered?
        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {
                for (int z = 0; z < size; z++) {
                    cell &cCell = c.cells[x][y][z];
                    if(cCell.transparent){
                        std::fill(cCell.sides, cCell.sides + 6, false);
                        continue;
                    } else {
                        // check if adjascent blocks are transparent
                        // chunk are assumed to be transparent for now
                        //                 chunk edges     ||  adjascent blocks
                        cCell.sides[0] = (x == size - 1) ||  c.cells[x + 1][y][z].transparent;
                        cCell.sides[1] = (y == size - 1) ||  c.cells[x][y + 1][z].transparent;
                        cCell.sides[2] = (z == size - 1) ||  c.cells[x][y][z + 1].transparent;                               
                        cCell.sides[3] = (x == 0)        ||  c.cells[x - 1][y][z].transparent;     
                        cCell.sides[4] = (y == 0)        ||  c.cells[x][y - 1][z].transparent;
                        cCell.sides[5] = (z == 0)        ||  c.cells[x][y][z - 1].transparent;    
                    } 
                }
            }
        }
    }
    
    meshData generateMeshData(cell &c, meshData &mesh, int &dInd) {

        std::cout << "Generating mesh...";
        // // first, count how many vertices we need
        // int sCount = 0;
        // for (int x = 0; x < size; x++) {
        //     for (int y = 0; y < size; y++) {
        //         for (int z = 0; z < size; z++) {
        //             if (c.cells[x][y][z].transparent || cell.octave > 0){
        //                 continue;
        //             } else {
        //                 for (bool b : c.cells[x][y][z].sides) {
        //                     sCount += b;
        //                 }
        //             }
        //         }
        //     }
        // }
        
        // static int dCount = sCount * 18; // 2 tris * 3 vertices * 3 floats

        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {
                for (int z = 0; z < size; z++) {
                    cell &cCell = c.cells[x][y][x];
                    if (cCell.transparent){
                        continue;
                    } else if (cCell.octave > 0) {
                        generateMeshData(cCell, mesh, dInd);
                    } else {
                        static int sCount = 0;
                        for (bool b : cCell.sides) {
                            sCount += b;
                        }
                        mesh.verts += sCount*6; // 1 side = 2 tris = 6 verts
                        mesh.vData.resize(dInd + sCount*18); // 1 side = 6 verts = 18 floats

                        // +x
                        if(cCell.sides[0]){

                            mesh.vData[dInd]    = x+1.0f; // triangle 1
                            mesh.vData[dInd+1]  = y+1.0f;
                            mesh.vData[dInd+2]  = z+1.0f;
                            
                            mesh.vData[dInd+3]  = x+1.0f;
                            mesh.vData[dInd+4]  = y;     
                            mesh.vData[dInd+5]  = z+1.0f;

                            mesh.vData[dInd+6]  = x+1.0f;
                            mesh.vData[dInd+7]  = y;     
                            mesh.vData[dInd+8]  = z;     

                            mesh.vData[dInd+9]  = x+1.0f; // triangle 2
                            mesh.vData[dInd+10] = y+1.0f;
                            mesh.vData[dInd+11] = z+1.0f;
                                                   
                            mesh.vData[dInd+12] = x+1.0f;
                            mesh.vData[dInd+13] = y;     
                            mesh.vData[dInd+14] = z;     
                                                   
                            mesh.vData[dInd+15] = x+1.0f;
                            mesh.vData[dInd+16] = y+1.0f;
                            mesh.vData[dInd+17] = z;     

                            dInd += 18;
                        }

                        // +y
                        if(cCell.sides[1]){
                            
                            mesh.vData[dInd]    = x; // triangle 1
                            mesh.vData[dInd+1]  = y+1.0f;
                            mesh.vData[dInd+2]  = z;
                                                   
                            mesh.vData[dInd+3]  = x;
                            mesh.vData[dInd+4]  = y+1.0f;
                            mesh.vData[dInd+5]  = z+1.0f;
                                                   
                            mesh.vData[dInd+6]  = x+1.0f;
                            mesh.vData[dInd+7]  = y+1.0f;
                            mesh.vData[dInd+8]  = z+1.0f;
                                                   
                            mesh.vData[dInd+9]  = x; // triangle 2
                            mesh.vData[dInd+10] = y+1.0f;
                            mesh.vData[dInd+11] = z;
                                                   
                            mesh.vData[dInd+12] = x+1.0f;
                            mesh.vData[dInd+13] = y+1.0f;
                            mesh.vData[dInd+14] = z+1.0f;
                                                   
                            mesh.vData[dInd+15] = x+1.0f;
                            mesh.vData[dInd+16] = y+1.0f;
                            mesh.vData[dInd+17] = z;

                            dInd += 18;
                        }
                        
                        // +z
                        if(cCell.sides[2]){
                            
                            mesh.vData[dInd]    = x; // triangle 1
                            mesh.vData[dInd+1]  = y+1.0f;
                            mesh.vData[dInd+2]  = z+1.0f;
                                                   
                            mesh.vData[dInd+3]  = x;
                            mesh.vData[dInd+4]  = y;
                            mesh.vData[dInd+5]  = z+1.0f;
                                                   
                            mesh.vData[dInd+6]  = x+1.0f;
                            mesh.vData[dInd+7]  = y;
                            mesh.vData[dInd+8]  = z+1.0f;
                                                   
                            mesh.vData[dInd+9]  = x; // triangle 2
                            mesh.vData[dInd+10] = y+1.0f;
                            mesh.vData[dInd+11] = z+1.0f;
                                                   
                            mesh.vData[dInd+12] = x+1.0f;
                            mesh.vData[dInd+13] = y;
                            mesh.vData[dInd+14] = z+1.0f;
                                                   
                            mesh.vData[dInd+15] = x+1.0f;
                            mesh.vData[dInd+16] = y+1.0f;
                            mesh.vData[dInd+17] = z+1.0f;

                            dInd += 18;
                        }
                        
                        // -x
                        if(cCell.sides[3]){
                            
                            mesh.vData[dInd]    = x; // triangle 1
                            mesh.vData[dInd+1]  = y;
                            mesh.vData[dInd+2]  = z;
                                                   
                            mesh.vData[dInd+3]  = x;
                            mesh.vData[dInd+4]  = y+1.0f;
                            mesh.vData[dInd+5]  = z;
                                                   
                            mesh.vData[dInd+6]  = x;
                            mesh.vData[dInd+7]  = y+1.0f;
                            mesh.vData[dInd+8]  = z+1.0f;
                                                   
                            mesh.vData[dInd+9]  = x; // triangle 2
                            mesh.vData[dInd+10] = y;
                            mesh.vData[dInd+11] = z;
                                                   
                            mesh.vData[dInd+12] = x;
                            mesh.vData[dInd+13] = y+1.0f;
                            mesh.vData[dInd+14] = z+1.0f;
                                                   
                            mesh.vData[dInd+15] = x;
                            mesh.vData[dInd+16] = y;
                            mesh.vData[dInd+17] = z+1.0f;

                            dInd += 18;
                        }

                        // -y
                        if(cCell.sides[4]){
                            
                            mesh.vData[dInd]    = x+1.0f; // triangle 1
                            mesh.vData[dInd+1]  = y;
                            mesh.vData[dInd+2]  = z+1.0f;
                                                   
                            mesh.vData[dInd+3]  = x+1.0f;
                            mesh.vData[dInd+4]  = y;
                            mesh.vData[dInd+5]  = z;
                                                   
                            mesh.vData[dInd+6]  = x;
                            mesh.vData[dInd+7]  = y;
                            mesh.vData[dInd+8]  = z;
                                                   
                            mesh.vData[dInd+9]  = x+1.0f; // triangle 2
                            mesh.vData[dInd+10] = y;
                            mesh.vData[dInd+11] = z+1.0f;
                                                   
                            mesh.vData[dInd+12] = x;
                            mesh.vData[dInd+13] = y;
                            mesh.vData[dInd+14] = z;
                                                   
                            mesh.vData[dInd+15] = x;
                            mesh.vData[dInd+16] = y;
                            mesh.vData[dInd+17] = z+1.0f;

                            dInd += 18;
                        }
                        
                        // -z
                        if(cCell.sides[5]){
                            
                            mesh.vData[dInd]    = x+1.0f; // triangle 1
                            mesh.vData[dInd+1]  = y;
                            mesh.vData[dInd+2]  = z;
                                                   
                            mesh.vData[dInd+3]  = x+1.0f;
                            mesh.vData[dInd+4]  = y+1.0f;
                            mesh.vData[dInd+5]  = z;
                                                   
                            mesh.vData[dInd+6]  = x;
                            mesh.vData[dInd+7]  = y+1.0f;
                            mesh.vData[dInd+8]  = z;
                                                   
                            mesh.vData[dInd+9]  = x+1.0f; // triangle 2
                            mesh.vData[dInd+10] = y;
                            mesh.vData[dInd+11] = z;
                                                   
                            mesh.vData[dInd+12] = x;
                            mesh.vData[dInd+13] = y+1.0f;
                            mesh.vData[dInd+14] = z;
                                                   
                            mesh.vData[dInd+15] = x;
                            mesh.vData[dInd+16] = y;
                            mesh.vData[dInd+17] = z;
                            
                            dInd += 18;
                        }
                    }
                }
            }
        }

        for (int i = 0; i < mesh.vData.size(); i++){

            mesh.cData[i] = mesh.vData[i] / size - (1/3) + (rand() / double(RAND_MAX));
        }
        
        return(mesh);
    }


    meshData generateMeshData() {
        meshData mesh;
        int ind = 0;
        return(generateMeshData(planet, mesh, ind));
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
    
    Gaia world(4, 2, 2);

    std::cout << "done generating world";
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
