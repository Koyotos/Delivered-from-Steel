#include "include/ResourceManager/ResourceManager.hpp"

void ResourceManager::ConfigurePaths() {
    path pth = Globals::GetGlobals().GetExecDir()/"res";
    modelsPath = pth/"models";
    shadersPath = pth/"shaders";
    spritesPath = pth/"sprites";
    audioPath = pth/"sounds";
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

void ResourceManager::ApplyAssetsSFX(shared_ptr<Node> node, unordered_map<string,std::any> data) {
    auto resolveAudioPath = [&](const string& name) -> string {
		path oggPath = audioPath / (name + ".ogg");
        if (filesystem::exists(oggPath)) {
			return oggPath.string();
        }
		return (audioPath / (name + ".wav")).string();
	};
    if(data.contains("sound") && audioManager) {
        string soundName = fromMap(string, "sound", data);
        audioManager->LoadSound(soundName, resolveAudioPath(soundName));
    }
    if(data.contains("sounds") && audioManager) {
        vector<std::any> soundList = fromMap(vector<std::any>, "sounds", data);
        for (const auto& soundAny : soundList) {
            string soundName = std::any_cast<string>(soundAny);
            audioManager->LoadSound(soundName, resolveAudioPath(soundName));
        }
    }
}

void ResourceManager::ApplyAssetsGFX(shared_ptr<Node> node, unordered_map<string,std::any> data) {
    if(node->Type() == "Node") {
        return;
    }
   
    if(data.contains("shader")) {
        shared_ptr<VisualNode> cast = static_pointer_cast<VisualNode>(node);
        shared_ptr<Shader> sh = LoadShader(fromMap(string,"shader",data));
        cast->SetShader(LoadShader(fromMap(string,"shader",data)));
        currentlyLoading->sceneShaders.push_back(sh);
    }

    if(node->RenderType() == 4) {
        auto cast = static_pointer_cast<Object3D>(node);
        if(data.contains("model")) {
            shared_ptr<Model> mdl = LoadModel(fromMap(string,"model",data));
            cast->SetModel(mdl);
            currentlyLoading->sceneModels.push_back(mdl);
        }

    } else if(node->RenderType() >= 3) {
        auto cast = static_pointer_cast<Object2D>(node);
        if(data.contains("sprite")) {
            shared_ptr<Sprite> spr = LoadSprite(fromMap(string,"sprite",data));
            cast->SetSprite(spr);
            currentlyLoading->sceneSprites.push_back(spr);
            if (data.contains("playAnimation")) {
                float speed = data.contains("animSpeed") ? fromMap(float, "animSpeed", data) : 0.1f;
                bool loop = data.contains("animLoop") ? fromMap(bool, "animLoop", data) : true;
                cast->Play(fromMap(string, "playAnimation", data), speed, loop);
            }
        }
    } 
}

void ResourceManager::ManageAudio(unordered_map<string,std::any> data) {
    if(data.contains("bgm") && audioManager) {
            string bgmName = fromMap(string, "bgm", data);
            audioManager->RegisterBGM(bgmName, (audioPath / (bgmName + ".ogg")).string());
        }
        if (data.contains("bgms") && audioManager) {
            vector<std::any> bgmList = fromMap(vector<std::any>, "bgms", data);
            for (const auto& bgmAny : bgmList) {
                string bgmName = std::any_cast<string>(bgmAny);
                audioManager->RegisterBGM(bgmName, (audioPath / (bgmName + ".ogg")).string());
            }
        }
        if (data.contains("playlist") && audioManager) {
            vector<std::any> playlistAny = fromMap(vector<std::any>, "playlist", data);
            for (const auto& song : playlistAny) {
                currentlyLoading->scenePlaylist.push_back(std::any_cast<string>(song));
            }
        }
        if (data.contains("ambient") && audioManager) {
            currentlyLoading->sceneAmbient = fromMap(string, "ambient", data);
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
                ApplyAssetsGFX(node, objVarList);
                ApplyAssetsSFX(node, objVarList);
				ManageAudio(objVarList);
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
    auto& mVec =  currentlyLoading->sceneModels;
    auto resized = unique(mVec.begin(), mVec.end());
    mVec.resize(std::distance(mVec.begin(), resized));

    auto& sVec =  currentlyLoading->sceneSprites;
    auto resized2 = unique(sVec.begin(), sVec.end());
    sVec.resize(std::distance(sVec.begin(), resized2));

    auto& shVec =  currentlyLoading->sceneShaders;
    auto resized3 = unique(shVec.begin(), shVec.end());
    shVec.resize(std::distance(shVec.begin(), resized3));
    scene->SetRoot(root);
}

shared_ptr<Scene> ResourceManager::LoadScene(const path& scenePath) noexcept {
    if(!exists(scenePath)) {
        Globals::GetGlobals().Log("Non existend scene");
        return nullptr;
    }

    currentlyLoading = make_shared<Scene>();
    
    try {
        unordered_map<string, std::any> sceneData = LoadJSON(scenePath);
        currentlyLoading->name = fromMap(string,"name",sceneData);
        sceneData.erase(sceneData.find("name"));
        auto nodes = ParseNodes(sceneData);
        LinkScene(nodes, currentlyLoading);
        return currentlyLoading;
      } catch(const exception& except) {
        Globals::GetGlobals().Log(string("Can't load scene : ") + except.what());
        return nullptr;
    }
}

void ResourceManager::LoadSceneAsync(const path& path) noexcept {
    sceneAsyncQueue.push_back(async(launch::async, &ResourceManager::LoadScene, this, path.string()));
}

shared_ptr<Scene> ResourceManager::GetLoadedAsync(const string& name) noexcept {
    if (sceneAsyncQueue.empty()) {
        return nullptr;
    }
    if(sceneAsyncQueue.front().wait_for(seconds(0)) == future_status::ready) {
        shared_ptr<Scene> ret =  sceneAsyncQueue.front().get();
        sceneAsyncQueue.erase(sceneAsyncQueue.begin());
        return ret;
    }
    return nullptr;
}

void ResourceManager::UnloadScene(shared_ptr<Scene> scene) {
    for(auto& model : scene->sceneModels) {
        for(uint16_t i = 0; i < models.size(); i++) {
            if(model == models[i].model) {
                models[i].refCount--;
                if(models[i].refCount == 0) {
                    models.erase(models.begin()+i);
                }
            }
        }
    }

    for(auto& shader : scene->sceneShaders) {
        for(uint16_t i = 0; i < shaders.size(); i++) {
            if(shader == shaders[i].shader) {
                shaders[i].refCount--;
                if(shaders[i].refCount == 0) {
                    shaders.erase(shaders.begin()+i);
                }
            }
        }
    }

    for(auto& sprite : scene->sceneSprites) {
        for(uint16_t i = 0; i < sprites.size(); i++) {
            if(sprite == sprites[i].sprite) {
                sprites[i].refCount--;
                if(sprites[i].refCount == 0) {
                    sprites.erase(sprites.begin()+i);
                }
            }
        }
    }

    for(uint8_t i = 0; i < scenes.size(); i++) {
        if(scenes[i] == scene) {
            scenes.erase(scenes.begin()+i);
            return;
        }
    }
}

ResourceManager::ResourceManager() {

}

ResourceManager::~ResourceManager() {
    resourceStream.close();
}