#include "include/Renderer/Model.hpp"
#include "include/Renderer/Mesh.hpp"

const string& Model::GetDir() const noexcept {
    return directory;
}

void Model::Draw(Shader& sh) {
    for(auto& k : meshes) {
        k->Draw(sh);
    }
}

void Model::ProcessNode(aiNode *node, const aiScene *scene) {
    for(GLuint i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(ProcessMesh(mesh, scene));			
    }
    for(GLuint i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }
}

shared_ptr<Mesh> Model::ProcessMesh(aiMesh *mesh, const aiScene *scene) {
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    for(GLuint i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.position = GLMVec(mesh->mVertices[i]);
        if(mesh->mNormals) {
            vertex.normal = GLMVec(mesh->mNormals[i]);
        }

        if(mesh->mTextureCoords[0]) {
            vertex.uv = GLMVec(mesh->mTextureCoords[0][i]);
        }

        vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }  

    if(mesh->mMaterialIndex >= 0) {
        if(mesh->mMaterialIndex >= 0) {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }  
    }

    return make_shared<Mesh>(vertices, indices, textures, false); // TODO
}

vector<Texture> Model::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName) {
    vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for(unsigned int j = 0; j < texturesLoaded.size(); j++) {
            if(strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(texturesLoaded[j]);
                skip = true; 
                break;
            }
        }
        if(skip) continue;
        Texture texture;
        texture.id = TextureFromFile(str.C_Str(), directory.c_str());
        texture.type = typeName;
        texture.path = string(str.C_Str());
        textures.push_back(texture);
        texturesLoaded.push_back(texture);
    }
    return textures;
}

Model::Model(string path) {
    Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        auto dupa = string(importer.GetErrorString());
        throw runtime_error("ASSIMP ERROR : " + string(importer.GetErrorString()));
    }
    directory = path.substr(0, path.find_last_of('/'));

    ProcessNode(scene->mRootNode, scene);
}