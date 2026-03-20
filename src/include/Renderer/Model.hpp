#ifndef FE_MODEL
#define FE_MODEL

#include "include/Renderer/Mesh.hpp"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include <stb_image.h>
#include <stdexcept>
#include <map>
#include <memory>
#include <vector>

using namespace Assimp;

class Model {
    private:
    string directory;
    vector<shared_ptr<Mesh>> meshes;
    vector<Texture> texturesLoaded;

    void LoadModel(const string&);
    void ProcessNode(aiNode *node, const aiScene *scene);
    shared_ptr<Mesh> ProcessMesh(aiMesh *mesh, const aiScene *scene);
    vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
    
    public:
    Model(string);
    void Draw(Shader &shader);	
};

#endif