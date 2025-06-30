#include "shader.hpp"
#include <iostream>

CLASS_DEFINITION(Asset, Shader)


Shader::Shader(std::string && initialName)
    : Asset(std::move(initialName)) {}

Shader::Shader(std::string && initialName, std::string && v, std::string && f)
    : Asset(std::move(initialName)), vertexFile(std::move(v)), fragmentFile(std::move(f)) {
    init(); //TODO: get rid of this and have new assets added in another function that automatically runs init
}

void Shader::serialize(Archive& archive) {
    Asset::serialize(archive);
    archive & vertexFile & fragmentFile;
}

void Shader::init() {
    if (!(ID = LoadShaders(vertexFile.c_str(), fragmentFile.c_str()))) {
        std::cerr << "Failed to load shader." << std::endl;
    }
}

void Shader::Activate() {
    glUseProgram(ID);
}

void Shader::Delete() {
    glDeleteProgram(ID);
}
