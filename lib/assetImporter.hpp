#ifndef DASSETIMPORTER_HPP
#define DASSETIMPORTER_HPP

#include "../external/assimp/Importer.hpp"
#include "../external/assimp/scene.h"
#include "../external/assimp/postprocess.h"

// ideas for process: 
//  - create filethingy derived asset
//      - this can have some base class that contains child assets, like an asset pack
//  - create assets for each part 
//      - eg. fbx contains multiple meshes, materials, etc.
//  - for more complex assets (fbx), create thingy with component that handles importing
//  - import component:
//      - has file asset & import settings
//      - has an unpack function and auto-unpack (toggleable) to build thingy hierarchy automatically

bool importFBX( const std::string& pFile ) {
    // Create an instance of the Importer class
    Assimp::Importer importer;

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // probably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile( pFile,
        aiProcess_CalcTangentSpace       |
        aiProcess_Triangulate            |
        aiProcess_JoinIdenticalVertices  |
        aiProcess_SortByPType);

    // If the import failed, report it
    if (scene == nullptr) {
        // TODO: implement error logging system
        // DoTheErrorLogging( importer.GetErrorString());
        return false;
    }

    // Now we can access the file's contents.
    DoTheSceneProcessing(scene);

    // We're done. Everything will be cleaned up by the importer destructor
    return true;
}

#endif
