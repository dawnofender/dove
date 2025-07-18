#include "mesh.hpp"

CLASS_DEFINITION(Asset, Mesh)

CLASS_DEFINITION(Thing, MeshLayer)
CLASS_DEFINITION(MeshLayer, Vec4Layer)
CLASS_DEFINITION(MeshLayer, Vec3Layer)
CLASS_DEFINITION(MeshLayer, Vec2Layer)
CLASS_DEFINITION(MeshLayer, IndexLayer) // unused probably
CLASS_DEFINITION(Thing, FaceGroup) // unused probably

Mesh::Mesh(std::string && initialName) 
    : Asset(std::move(initialName)) {}

void Mesh::serialize(Archive& archive) {
    Asset::serialize(archive);
    archive &
        layers &
        indices;
}
