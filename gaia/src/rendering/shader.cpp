#include "shader.hpp"

Shader::Shader(const char* vertexFile, const char* fragmentFile) {
  ID = LoadShaders(vertexFile, fragmentFile);
}

void Shader::Activate() {
    glUseProgram(ID);
}

void Shader::Delete() {
    glDeleteProgram(ID);
}
