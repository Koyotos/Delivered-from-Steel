import sys
import json
import math
import os
import re

filename = sys.argv[1]
sceneName = sys.argv[2]
nodeData = list()
nodes = list()
idlist = dict()
idCounter : int = 0
ext_resources = dict()

class Node:
    __slots__ = ("id","parent","data", "is_prefab_child")
    def __init__(self, id, parent, data, is_prefab_child=False):
        self.id = id
        self.data = data
        self.parent = parent
        self.is_prefab_child = is_prefab_child
        
    def ToDict(self):
        p_id = 0
        try:
            p_id = int(self.parent)
        except ValueError:
            p_id = 0

        ret = {
            "id" : self.id,
            "parent" : p_id
        }
        ret.update(self.data)
        return ret

def GetTransform(line):
    valStartIdx = line.find('(')
    valList = line[(valStartIdx+1):]
    valList = valList[:(len(valList)-2)]
    converted = list()
    for value in valList.split(','):
        converted.append(float(value))
        
    px, py, pz = converted[9], converted[10], converted[11]
    
    det = converted[0] * (converted[4] * converted[8] - converted[5] * converted[7]) - \
          converted[1] * (converted[3] * converted[8] - converted[5] * converted[6]) + \
          converted[2] * (converted[3] * converted[7] - converted[4] * converted[6])

    sx = math.sqrt(converted[0]**2 + converted[1]**2 + converted[2]**2)
    sy = math.sqrt(converted[3]**2 + converted[4]**2 + converted[5]**2)
    sz = math.sqrt(converted[6]**2 + converted[7]**2 + converted[8]**2)
    
    if det < 0:
        sy = -sy

    r00 = converted[0] / sx if sx != 0 else 0
    r01 = converted[1] / sx if sx != 0 else 0
    r02 = converted[2] / sx if sx != 0 else 0
    r10 = converted[3] / sy if sy != 0 else 0
    r11 = converted[4] / sy if sy != 0 else 0
    r12 = converted[5] / sy if sy != 0 else 0
    r20 = converted[6] / sz if sz != 0 else 0
    r21 = converted[7] / sz if sz != 0 else 0
    r22 = converted[8] / sz if sz != 0 else 0
    
    clamped_r20 = max(-1.0, min(1.0, -r20))
    ry = math.asin(clamped_r20)
    
    if abs(r20) != 1:
        rx = math.atan2(r21, r22)
        rz = math.atan2(r10, r00)
    else:
        rx = 0
        rz = math.atan2(-r01, r11)

    def clean(val):
        return 0.0 if abs(val) < 1e-5 else round(val, 5)

    return "transform", [clean(px), clean(py), clean(pz), 
                         clean(rx), clean(ry), clean(rz), 
                         clean(sx), clean(sy), clean(sz)]

def ConvertType(value: str):
    value = value.strip()
    if value.lower() == "true": return True
    if value.lower() == "false": return False
    if value.lower() == "null": return None
    
    try: return int(value)
    except ValueError: pass
    
    try: return float(value)
    except ValueError: pass
    
    if value.startswith("[") and value.endswith("]"):
        try: return json.loads(value)
        except ValueError: pass
    
    return value

def GetHeader(line):
    objName = ""
    objParent = ""
    instanceId = ""
    
    name_match = re.search(r'name="([^"]+)"', line)
    if name_match: objName = name_match.group(1)
    
    parent_match = re.search(r'parent="([^"]+)"', line)
    if parent_match: objParent = parent_match.group(1)
        
    instance_match = re.search(r'instance=ExtResource\("([^"]+)"\)', line)
    if instance_match: instanceId = instance_match.group(1)
        
    return objName, objParent, instanceId

def GetMeta(line):
    separated = line.split('=', 1)
    metaName = separated[0].split('/')[1].strip()
    rawValue = separated[1].strip(' "\n')
    metaValue = ConvertType(rawValue)
    return metaName, metaValue

def resolve_godot_path(res_path):
    clean_path = res_path.replace("res://", "").replace("/", os.sep)
    target_file_name = os.path.basename(clean_path)
    current_dir = os.path.dirname(os.path.abspath(filename))
    
    project_root = current_dir
    while True:
        if os.path.exists(os.path.join(project_root, "project.godot")):
            full_path = os.path.join(project_root, clean_path)
            if os.path.exists(full_path): return full_path
            break
        parent = os.path.dirname(project_root)
        if parent == project_root:
            break
        project_root = parent
        
    fallback_path = os.path.join(current_dir, clean_path)
    if os.path.exists(fallback_path): return fallback_path
    if os.path.exists(clean_path): return clean_path
    
    search_root = project_root if os.path.exists(os.path.join(project_root, "project.godot")) else current_dir
    for root, dirs, files in os.walk(search_root):
        if target_file_name in files:
            return os.path.join(root, target_file_name)
            
    return None

def ParseNode():
    global idCounter
    header = nodeData.pop(0)
    nName, nParent, nInstance = GetHeader(header)
    nData = dict()
    
    is_prefab = False
    
    if nInstance and nInstance in ext_resources:
        res_path = ext_resources[nInstance]
        prefab_path = resolve_godot_path(res_path)
        
        if prefab_path:
            try:
                prefab_nodes_raw = []
                current_p_node = None
                with open(prefab_path, "r", encoding="utf-8") as pf:
                    for pf_line in pf:
                        if pf_line.startswith("[node "):
                            cName, cParent, _ = GetHeader(pf_line)
                            current_p_node = {"name": cName, "parent": cParent, "data": {}}
                            prefab_nodes_raw.append(current_p_node)
                        elif current_p_node is not None:
                            if pf_line.find("metadata/") >= 0:
                                n, v = GetMeta(pf_line)
                                current_p_node["data"][n] = v
                            elif pf_line.find("transform") >= 0:
                                n, v = GetTransform(pf_line)
                                current_p_node["data"][n] = v

                if prefab_nodes_raw:
                    is_prefab = True
                    prefab_root = prefab_nodes_raw[0]
                    
                    nData.update(prefab_root["data"])
                    
                    for line in nodeData:
                        if line.find("metadata/") >= 0:
                            n, v = GetMeta(line)
                            nData[n] = v
                        elif line.find("transform") >= 0:
                            n, v = GetTransform(line)
                            nData[n] = v
                            
                    if nParent == "0" or nParent == "":
                        idlist[nName] = 0
                    elif nParent == '.':
                        idlist[nName] = idCounter
                        nParent = "0"
                    else:
                        idlist[nName] = idCounter

                    for i in idlist:
                        if nParent == i:
                            nParent = idlist[i]
                            
                    root_id = idCounter
                    idCounter += 1
                    nodes.append(Node(nName, nParent, nData))
                    
                    valid_children = []
                    for pn in prefab_nodes_raw[1:]:
                        if "class" in pn["data"]:
                            valid_children.append(pn)
                            
                    prefab_path_to_id = {".": root_id}
                    
                    for child in valid_children:
                        child_id = idCounter
                        idCounter += 1
                        
                        cParent_path = child["parent"]
                        cName = child["name"]
                        if cParent_path == ".":
                            full_path = cName
                        else:
                            full_path = f"{cParent_path}/{cName}"
                            
                        prefab_path_to_id[full_path] = child_id
                        child["new_id"] = child_id
                        
                    for child in valid_children:
                        c_parent_path = child["parent"]
                        c_parent_id = prefab_path_to_id.get(c_parent_path, root_id)
                        
                        nodes.append(Node(child["new_id"], c_parent_id, child["data"], is_prefab_child=True))
                        
            except Exception as e:
                nData["DEBUG_ERROR"] = f"Error reading: {str(e)}"
        else:
            nData["DEBUG_ERROR"] = f"Cant find: {res_path}"

    if not is_prefab:
        for line in nodeData:
            if line.find("metadata/") >= 0:
                n, v = GetMeta(line)
                nData[n] = v
            elif line.find("transform") >= 0:
                n, v = GetTransform(line)
                nData[n] = v
                
        if nParent == "0" or nParent == "":
            idlist[nName] = 0
        elif nParent == '.':
            idlist[nName] = idCounter
            nParent = "0"
        else:
            idlist[nName] = idCounter
            
        for i in idlist:
            if nParent == i:
                nParent = idlist[i]
                
        idCounter += 1
        nodes.append(Node(nName, nParent, nData))

firstNode = False   

with open(filename, "r", encoding="utf-8") as gdScene:
    while True:
        line = gdScene.readline()
        if not line:
            if firstNode:
                ParseNode()
            break
        if line == "\n":
            continue
            
        if line.startswith("[ext_resource"):
            path_match = re.search(r' path="([^"]+)"', line)
            id_match = re.search(r' id="([^"]+)"', line)
            if path_match and id_match:
                ext_resources[id_match.group(1)] = path_match.group(1)
            
        if line.startswith("[node "):
            if firstNode:
                ParseNode()
            nodeData.clear()
            nodeData.append(line)
            firstNode = True
        elif firstNode:
            nodeData.append(line)

jsonDict = {
    "name" : sceneName
}

for n in nodes:
    nDict = n.ToDict()
    nId = nDict.pop("id")
    
    if not n.is_prefab_child:
        nId = idlist.get(nId, nId)
    
    if nId == 0:
        nId += 1
        nDict["class"] = "Node"
        nDict["process"] = True
        nDict["input"] = False
        nDict["physics"] = False
    else:
        nId += 1
        nDict["parent"] += 1
        
    if nDict.get("class") == "Light" and "transform" in nDict:
        del nDict["transform"]
        
    jsonDict.update({str(nId) : nDict})

with open(sceneName+".json", "w", encoding="utf-8") as parsed:
    json.dump(jsonDict, parsed, indent=4)