#ifndef FE_MODEL
#define FE_MODEL

#include "include/Renderer/Mesh.hpp"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include <stb_image.h>
#include <stdexcept>
#include <map>
#include <vector>

class Model {
    private:
    string directory;
    vector<Mesh> meshes;
    vector<Texture> texturesLoaded;
    std::map<string, BoneInfo> boneInfoMap;
    int boneCounter = 0;

    void LoadModel(const string&);
    void ProcessNode(aiNode *node, const aiScene *scene);
    Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
    GLuint TextureFromFile(const char*, const char*);
    vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
    void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
    void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
    
    public:
    Model(char *path);
    void Draw(Shader &shader);	
};

#endif