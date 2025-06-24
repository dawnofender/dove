#include "shader.hpp"
#include <iostream>

Shader::Shader(const char* vertexFile, const char* fragmentFile) {
    if (!(ID = LoadShaders(vertexFile, fragmentFile))) {
        std::cerr << "Failed to load shader." << std::endl;
    }
}

void Shader::Activate() {
    glUseProgram(ID);
}

void Shader::Delete() {
    glDeleteProgram(ID);
}
