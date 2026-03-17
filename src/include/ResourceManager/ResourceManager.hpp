#ifndef FE_RESOURCE_MANAGER
#define FE_RESOURCE_MANAGER

#include "include/Renderer/Model.hpp"
#include <vector>

using namespace std;

class ResourceManager {
    private:
    vector<Model> models;
    vector<Shader> shader;
    
    public:
};

#endif