#include "include/ResourceManager/ResourceManager.hpp"
#include "include/Core/Node.hpp"
#include "include/Globals/Globals.hpp"
#include "include/Renderer/TextNode.hpp"
#include <cstdint>
#include <filesystem>


void ResourceManager::ConfigurePaths() {
    path pth = Globals::GetGlobals().GetExecDir()/"res";
    modelsPath = pth/"models";
    shadersPath = pth/"shaders";
    spritesPath = pth/"sprites";
    audioPath = pth/"audio";
}

string ResourceManager::LoadPlainText(const path& p) {
    resourceStream.open(p, ios::in);
    if(!resourceStream.good()) {
        throw runtime_error("Can't open file!");
    }
    string str(istreambuf_iterator<char>{resourceStream}, {});
    resourceStream.close();
    return str;
}

std::any ResourceManager::JSONtoAny(const json& val) {
    if(val.is_object()) {
        unordered_map<string, std::any> obj;
        for(auto&[k,v] : val.items()) {
            obj.emplace(k, JSONtoAny(v));
        }
        return obj;
    } else if(val.is_array()) {
        vector<std::any> arr;
        arr.reserve(val.size());
        for(const auto& el : val) {
            arr.emplace_back(JSONtoAny(el));
        }
        return arr;
    } else if(val.is_string()) {
        return val.get<string>();
    } else if(val.is_boolean()) {
        return val.get<bool>();
    } else if(val.is_number_integer()) {
        return val.get<int64_t>();
    } else if(val.is_number_float()) {
        return val.get<float>();
    } else if(val.is_null()) {
        return nullptr;
    }
    return val.dump();
}

unordered_map<string, std::any> ResourceManager::LoadJSON(const path& jsonFile) {
    stringstream ss(LoadPlainText(jsonFile));
    if(!json::accept(ss)) {
        throw runtime_error("Invalid JSON in " + jsonFile.string());
    }

    ss.clear();
    ss.seekg(0, std::ios::beg);
    json root = json::parse(ss);

    unordered_map<string, std::any> result;
    for(auto&[key,val] : root.items()) {
        result.emplace(key, JSONtoAny(val));
    }
    return result;
}

shared_ptr<Model> ResourceManager::LoadModel(const string& name) {
    for(auto& loadedEntry : models) {
        if(loadedEntry.model->GetDir() == modelsPath/name) {
            loadedEntry.refCount++;
            return loadedEntry.model;       
        }
    }
    RefCountModel newRCM;
    newRCM.model = make_shared<Model>((modelsPath / name / path(name + ".obj")).string());
    newRCM.refCount = 1;
    models.push_back(newRCM);
    return newRCM.model;
}

shared_ptr<Sprite> ResourceManager::LoadSprite(const string& name) {
    for(auto& loadedEntry : sprites) {
        if(loadedEntry.sprite->GetDir() == spritesPath/name) {
            loadedEntry.refCount++;
            return loadedEntry.sprite;       
        }
    }
    RefCountSprite newRCS;
    newRCS.sprite = make_shared<Sprite>((spritesPath/name).string());
    newRCS.refCount = 1;
    sprites.push_back(newRCS);
    return newRCS.sprite;
}

shared_ptr<Shader> ResourceManager::LoadShader(const string& name) {
    for(auto& loadedEntry : shaders) {
        if(loadedEntry.shader->GetName() == name) {
            loadedEntry.refCount++;
            return loadedEntry.shader;       
        }
    }

    string sources[4];
    RefCountShader newRCS;

    try {
        sources[0] = LoadPlainText(shadersPath/name/path(name + ".vert"));
        sources[1] = LoadPlainText(shadersPath/name/path(name + ".tes"));
        sources[2] = LoadPlainText(shadersPath/name/path(name + ".geo"));
        sources[3] = LoadPlainText(shadersPath/name/path(name + ".frag"));
        newRCS.shader = make_shared<Shader>(sources,name);
        newRCS.refCount = 1;
        shaders.push_back(newRCS);
        return newRCS.shader;
    } catch(const exception& except) {
        return nullptr;
    }
}

void ResourceManager::ApplyAssets(shared_ptr<Node> node, unordered_map<string,std::any> data) {
    if(auto obj3d = dynamic_pointer_cast<Object3D>(node)) { // Think about casts, try to replace 
        if(data.contains("model")) {
            obj3d->SetModel(LoadModel(fromMap(string,"model",data)));
        }

        if(data.contains("shader")) {
            obj3d->SetShader(LoadShader(fromMap(string,"shader",data)));
        }
    } else if(auto obj2d = dynamic_pointer_cast<Object2D>(node)) {
        if(data.contains("sprite")) {
            obj2d->SetSprite(LoadSprite(fromMap(string,"sprite",data)));
            obj2d->GetSprite()->SetActiveTexture(fromMap(string,"active",data));
        }
        if(data.contains("shader")) {
            obj2d->SetShader(LoadShader(fromMap(string,"shader",data)));
        }
    } else if(auto textNode = dynamic_pointer_cast<TextNode>(node)) {
        if(data.contains("shader")) {
            textNode->SetShader(LoadShader(fromMap(string,"shader",data)));
        }
    }
}

vector<tuple<shared_ptr<Node>, int64_t, int64_t>> ResourceManager::ParseNodes(unordered_map<string, std::any>& data) {
    vector<tuple<shared_ptr<Node>, int64_t, int64_t>> nodes;
    for(auto& [name, val] : data) {
        auto objVarList = any_cast<unordered_map<string,std::any>>(val);
        string currentType = fromMap(string,"class",objVarList);
        for(auto& octEntry : objectClassTable) {
            if(currentType==octEntry.first) {
                shared_ptr<Node> node = octEntry.second(objVarList);
                ApplyAssets(node, objVarList);
                nodes.push_back({node, fromMap(int64_t, "parent", objVarList), stoi(name)});
            }
        }
    }   
    return nodes;
}

void ResourceManager::LinkScene(vector<tuple<shared_ptr<Node>, int64_t, int64_t>>& nodes, shared_ptr<Scene> scene) {
	shared_ptr<Node> root = nullptr;
    for(auto& n : nodes) {
        for(auto& ppn : nodes) {
            if(get<1>(n) == get<2>(ppn)) {
                get<0>(ppn)->AddChild(get<0>(n));
            }
            if(get<1>(n) == 0) {
				root = get<0>(n);
            }
        }
    }
    scene->SetRoot(root);
}

shared_ptr<Scene> ResourceManager::LoadScene(const path& scenePath) noexcept {
    if(!exists(scenePath)) {
        Globals::GetGlobals().Log("Non existend scene");
        return nullptr;
    }

    shared_ptr<Scene> loadedScene = make_shared<Scene>();
    
    try {
        unordered_map<string, std::any> sceneData = LoadJSON(scenePath);
        loadedScene->name = fromMap(string,"name",sceneData);
        sceneData.erase(sceneData.find("name"));
        auto nodes = ParseNodes(sceneData);
        LinkScene(nodes, loadedScene);
        scenes.push_back(loadedScene);
        return loadedScene;
    } catch(const exception& except) {
        Globals::GetGlobals().Log(string("Can't load scene : ") + except.what());
        return nullptr;
    }
}

ResourceManager::ResourceManager() {

}

ResourceManager::~ResourceManager() {
    resourceStream.close();
}