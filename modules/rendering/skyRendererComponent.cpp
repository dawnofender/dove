#include "skyRendererComponent.hpp"

CLASS_DEFINITION(MeshRenderer, SkyRenderer)


SkyRenderer::SkyRenderer(std::string &&initialName)
    : MeshRenderer(std::move(initialName)) {}

SkyRenderer::SkyRenderer(std::string &&initialName, std::shared_ptr<Material> s, std::shared_ptr<Mesh> m)
    : MeshRenderer(std::move(initialName), s, m) {}

SkyRenderer::~SkyRenderer() {
    renderers.erase(this);
}

void SkyRenderer::init() {
    MeshRenderer::init();
    renderers.insert(this);
}


void SkyRenderer::drawAll(glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {

    // TODO: 
    //  - just use an inverted cube model instead of disabling backface culling
    //  - disable depth mask in shader 
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE); // backface culling
    //
    glm::mat4 skyViewMatrix = glm::mat4(glm::mat3(viewMatrix));
    for (auto &renderer : renderers) {
        renderer->draw(skyViewMatrix, projectionMatrix);
    }
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE); // backface culling
    unbindBufferData();
}

void SkyRenderer::draw(glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
    // TODO: if this returns false, there was an error, we can skip the object and render all errored objects last with the same shader 
    
    // material->Activate(glm::inverse(glm::mat4(glm::mat3(Camera::getViewMatrix()))));
    // material->Activate(glm::translate(glm::mat4(1), glm::vec3(glm::inverse(Camera::getViewMatrix())[3])));
    material->Activate(glm::mat4(0), viewMatrix, projectionMatrix);

    GLuint ViewMatrixID = glGetUniformLocation(material->getShader()->ID, "view");
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);

    bindBufferData();

    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr);
}
