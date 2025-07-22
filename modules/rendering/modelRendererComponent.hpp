#ifndef MODELRENDERERCOMPONENT_HPP
#define MODELRENDERERCOMPONENT_HPP

#include <set>

#include "meshRendererComponent.hpp"


class ModelRenderer : public MeshRenderer {
CLASS_DECLARATION(ModelRenderer)
private:


    // maybe make this a vector so people can layer materials? probably not great for performance but could be interesting...
    Transform *transform;
    Thingy *host;
    std::pair<glm::vec3, glm::vec3> bounds;

    std::vector<std::shared_ptr<Material>> materials;
    
    static inline std::set<ModelRenderer *> renderers;

protected:
    uint8_t state = 1;

public:
    ModelRenderer(std::string &&initialName = "ModelRenderer", Thingy *h = nullptr, std::shared_ptr<Material> s = nullptr, std::shared_ptr<Mesh> m = nullptr);

    virtual ~ModelRenderer();

    virtual void serialize(Archive& archive) override;
    virtual void init() override;
    
    // TODO: pass matrices by reference instead
    void draw(glm::mat4 viewMatrix = glm::mat4(0), glm::mat4 projectionMatrix = glm::mat4(0));
    void setBounds(glm::vec3 a, glm::vec3 b);

    static void drawAll(glm::mat4 viewMatrix = glm::mat4(0), glm::mat4 projectionMatrix = glm::mat4(0));
    static void deleteAll();
};

#endif
