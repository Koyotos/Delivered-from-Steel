#include "include/Renderer/Model.hpp"
#include "include/Renderer/Mesh.hpp"

void Model::Draw(Shader& sh) {
    for(auto& k : meshes) {
        k.Draw(sh);
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

Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene) {
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

    return Mesh(vertices, indices, textures, false); // TODO
}

GLuint Model::TextureFromFile(const char* name, const char* dir) {
    if(!name) {
        throw invalid_argument("Name is null");
    }
    if(!dir) {
        throw invalid_argument("Directory is null");
    }
    string path = string(dir) + "/" + string(name);
    int w, h, c;
    unsigned char* data = stbi_load(path.c_str(),&w,&h,&c,0);
    if(!data) {
        throw runtime_error("Can't read texture file");
    }
    GLenum format = GL_RGB;
    switch (c) {
        case 1: format = GL_RED; break;
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
        default: throw runtime_error("");
    }
    GLuint texID = 0;
    glGenTextures(1,&texID);
    glBindTexture(GL_TEXTURE_2D,texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return texID;
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

void Model::SetVertexBoneData(Vertex& vertex, int boneID, float weight) {
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
        if (vertex.boneIDs[i] < 0) {
            vertex.weights[i] = weight;
            vertex.boneIDs[i] = boneID;
            break;
        }
    }
}

void Model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene) {
    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCounter;
            newBoneInfo.offset = GLMMat4(mesh->mBones[boneIndex]->mOffsetMatrix); 
            boneInfoMap[boneName] = newBoneInfo;
            boneID = boneCounter;
            boneCounter++;
        }
        else {
            boneID = boneInfoMap[boneName].id;
        }
        if(boneID==-1){
            throw runtime_error("Invalid bone");
        }
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;
        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            if(vertexId > vertices.size()) {
                throw runtime_error("Invalid vertex id");
            }
            SetVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}