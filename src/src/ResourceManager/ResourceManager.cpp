#include "include/ResourceManager/ResourceManager.hpp"
#include "include/Core/Node.hpp"
#include "include/Globals/Globals.hpp"
#include <cstdint>
#include <filesystem>

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

shared_ptr<Model> ResourceManager::LoadModel() {

}

shared_ptr<Sprite> ResourceManager::LoadSprite() {

}

shared_ptr<Shader> ResourceManager::LoadShader() {

}

void ResourceManager::ApplyAssets(shared_ptr<Node>) {

}

vector<pair<shared_ptr<Node>, int64_t>> ResourceManager::ParseNodes(unordered_map<string, std::any>& data) {
    vector<pair<shared_ptr<Node>, int64_t>> nodes;
    for(auto& [name, val] : data) {
        auto objVarList = any_cast<unordered_map<string,std::any>>(val);
        string currentType = fromMap(string,"name",objVarList);
        for(auto& octEntry : objectClassTable) {
            if(currentType==octEntry.first) {
                shared_ptr<Node> node = octEntry.second(objVarList);
                ApplyAssets(node);
                nodes.push_back({node, fromMap(int64_t, "parent", objVarList)});
            }
        }
    }   
    return nodes;
}

void ResourceManager::LinkScene(vector<pair<shared_ptr<Node>, int64_t>>& nodes, shared_ptr<Scene> scene) {
    
}

void ResourceManager::LoadScene(const path& scenePath) noexcept {
    if(!exists(scenePath)) {
        Globals::GetGlobals().Log("Non existend scene");
        return;
    }

    shared_ptr<Scene> loadedScene = make_shared<Scene>();
    
    try {
        unordered_map<string, std::any> sceneData = LoadJSON(scenePath);
        loadedScene->name = fromMap(string,"name",sceneData);
        sceneData.erase(sceneData.find("name"));
        auto nodes = ParseNodes(sceneData);
        LinkScene(nodes, loadedScene);
        scenes.push_back(loadedScene);
    } catch(const exception& except) {
        Globals::GetGlobals().Log(string("Can't load scene : ") + except.what());
    }
}